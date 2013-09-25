// libuv wrapper
#include <cppcomponents_libuv/cppcomponents_libuv.hpp>

// For resumable and awaiter
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

// For stringstream
#include <sstream>

// For this_thread and sleep_for
#include <thread>
#include <chrono>

// use<Interface> wraps a cppcomponents interface for use
// (the reason this is needed is that you can also implement_interface<Interface> when implementing an interface)
using cppcomponents::use;

// Future<T> is just a template alias for use<IFuture<T>>
using cppcomponents::Future;

// Awaiter operator() uses boost::coroutine to emulate await on Future
using cppcomponents::awaiter;

// Resumable wraps a function taking awaiter into a function that returns Future
using cppcomponents::resumable;

// Like a Go channel (a little bit)
using cppcomponents::Channel;

// libuv types, Interfaces start with I
// Classes do not start with I and provide implementation of interfaces
using cppcomponents_libuv::ITty;
using cppcomponents_libuv::Tty;
using cppcomponents_libuv::TcpStream;
using cppcomponents_libuv::Timer;

// The general purpose libuv global methods are static methods under Uv
using cppcomponents_libuv::Uv;

// Use RAII to guarantee call to Uv::DefaultExecutor().MakeLoopExit();
using cppcomponents_libuv::LoopExiter;

using cppcomponents_libuv::ErrorCodes;


void fibonacci(std::uint16_t n, Channel < std::pair<std::uint16_t, std::uint64_t> > chan, Channel<int> stopchan){
	// First fibonacci is 0
	// So go ahead and write that as our first value
	auto fut = chan.Write({ 0, 0 });

	auto stopfut = stopchan.Read();

	// A vector to hold our intermediate calculations
	std::vector<std::uint64_t> fibs(n+1);

	
	if (n == 0){
		return;
	}
	if (n == 1){
		chan.Write({ 1, 1 });
		return;
	}

	// Set the 0th and 1st fibonacci numbers
	fibs[0] = 0;
	fibs[1] = 1;

	// Calculate all the fibonacci numbers up to and including n
	for (std::uint16_t i = 2; i <= n; ++i){
		// Fibonacci forumula
		fibs[i] = fibs[i - 2] + fibs[i - 1];

		// fut (the Future from chan.Write, will become ready when somebody reads the channel
		// that is a trigger that we need to write the latest value to the channel
		// Also, if we are done calculating (i==n) we write the value to the channel
		if (fut.Ready() || i==n || stopfut.Ready()){
			fut = chan.Write({ i, fibs[i] });
			if (stopfut.Ready()){
				return;
			}
		}
		
		
		// Slow us down so it looks like a long calculation, otherwise it runs too fast
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	

}
void handle_input(use<ITty> in, use<ITty> out, use<ITty> err, awaiter<void> await){
	// Write out the prompt
	await(out.Write("Type Quit and press return to quit,Fib to get the latest calculated fibonacci number, any other text to have it echoed\n"));

	// Start the fibonacci writer

	// First make a channel to communicate with the fibonacci calculating function
	auto fibchan = cppcomponents::make_channel < std::pair<std::uint16_t, std::uint64_t> >();
	auto stopchan = cppcomponents::make_channel<int>();
	// 93 is the largest fibonacci number that will fit in 64bits
	const std::uint16_t fibonacci_n = 93;

	// Uv::Async will run the function on the libuv threadpool
	Uv::Async(std::bind(fibonacci, fibonacci_n, fibchan,stopchan));

	// Start reading and get it as a channel
	auto chan = in.ReadStartWithChannel();
	
	// The value of the nth fibonacci number
	std::uint16_t n = 0;
	std::uint64_t value = 0;

	// Loop until we break
	while (true){
		// Read the channel and if we have Quit, then break, if Fib then output latest fibonacci (up to max 100 otherwise echo
		auto buf = await(chan.Read());
		std::string s(buf.Begin(),buf.End());
		if (s.substr(0, 4) == "Quit"){
			stopchan.Write(1);
			break;
		}
		else if (s.substr(0, 3) == "Fib"){
			std::stringstream s;

			// If n==fibonacci_n, then the calculation is already finished
			if (n < fibonacci_n){
				std::tie(n, value) = await(fibchan.Read());
			}

			s << "Fib(" << n << ") = " << value << "\n";
			await(out.Write(s.str()));
		}
		else{
			await(out.Write("You typed " + s));
		}

	}
	// Stop reading
	in.ReadStop();

}

void uv_main(awaiter<void> await){

	// Tty correspoding to stdin, stdout, stderr
	Tty in{ 0, true };
	Tty out{ 1, false };
	Tty err{ 2, false };

	// Dispatch our input handler
	auto quit_future = resumable<void>(handle_input)(in,out,err);

	// A Timer demo
	await(out.Write("Waiting 5 seconds before getting the url\n"));

	Timer timer;
	auto tc = timer.StartAsChannel(5000, 0);

	// You can type Quit to exit even if we are waiting for a timer
	await(when_any(tc.Read(),quit_future));

	if (quit_future.Ready()){
		return;
	}

	// Look up the address asynchronously
	auto addr = await(Uv::Getaddrinfo(std::string("www.nodejs.org"), "http", nullptr));

	// If we cannot find address
	if (addr == nullptr){
		await(err.Write("Address not found"));
		// Throw addrnotavailable as errorcode
		return cppcomponents::throw_if_error(static_cast<int>(ErrorCodes::Addrnotavail));
	}

	// Connect to the stream
	TcpStream stream;
	await(stream.Connect(addr->ai_addr));

	// Free the addr now that we no longer need it
	Uv::Freeaddrinfo(addr);

	// Send http get request to www.nodejs.org/about/
	std::string request = "GET /about/ HTTP/1.0\r\nHost: www.nodejs.org\r\n\r\n";

	await(stream.Write(request));


	// We get a channel instead of using a callback for reading
	auto chan = stream.ReadStartWithChannel();

	std::string response;

	while (true){

		// as_future allows us to get back a future that we can check the error code instead of throwing exception 
		auto fut = await.as_future(chan.Read());
		if (fut.ErrorCode() != 0){
			break;
		}

		// .Get gives us the results of the future
		auto buf = fut.Get();

		// Add what we read to our response
		response.append(buf.Begin(), buf.End());

	}

	// We are done reading so stop
	stream.ReadStop();

	// Output the response
	await(out.Write(response));

	// Wait unit we have typed quit
	await(quit_future);

}

#include <iostream>
int main(){

	resumable<void>(uv_main)().Then([](Future<void> f){
		// Exit loop at end of scope
		LoopExiter exiter;
		
		//Output any errors
		if (f.ErrorCode()){
			std::cerr << "Error in uv_main " << f.ErrorCode() << "\n";
		}

	});

	Uv::DefaultExecutor().Loop();

}