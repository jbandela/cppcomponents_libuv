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
// A template alias for Channel<T> = use<IChannel<T>>
using cppcomponents::Channel;

// Create a channel
using cppcomponents::make_channel;

//

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

			// Check if we were called to stop
			if (stopfut.Ready()){
				return;
			}
		}
		
		
		// Slow us down so it looks like a long calculation, otherwise it runs too fast
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	

}

void timer(use<ITty> out, std::uint64_t time, awaiter<void> await){


	Timer timer;
	auto chan = timer.StartAsChannel(time, 0);
	await(chan.Read());
	timer.Stop();
	std::stringstream s;
	s << "Finished waiting " << (double)(time) / 1000.0 << "seconds \n";
	out.Write(s.str());
}


std::string get_resource(const std::string& server,const std::string& port, const std::string& resource, awaiter<std::string> await){

	// Look up the address asynchronously
	auto addr = await(Uv::Getaddrinfo(server, port, nullptr));

	// If we cannot find address
	if (addr == nullptr){
		return "ERROR: Address not found\n";
	}

	// Connect to the stream
	TcpStream stream;
	await(stream.Connect(addr->ai_addr));

	// Free the addr now that we no longer need it
	Uv::Freeaddrinfo(addr);

	// Send http get request to www.nodejs.org/about/
	std::string request = "GET " + resource + " HTTP/1.0\r\nHost: " + server + "\r\n\r\n";

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

	return response;


}

void tcp_echo_server(int port, Channel<int> stopchan, awaiter<void> await){

	auto stopfut = stopchan.Read();
	TcpStream server;

	auto server_addr = Uv::Ip4Addr("127.0.0.1", port);

	server.Bind(server_addr);

	auto chan = server.ListenWithChannel(1);

	while (!stopfut.Ready()){
		auto listenfut = chan.Read();
		await(when_any(listenfut, stopfut));
		if (stopfut.Ready()){
			break;
		}
		auto serverstream = listenfut.Get();
		TcpStream client;
		serverstream.Accept(client);
		
		auto clientfunc = resumable<void>([client, stopfut](awaiter<void> await)mutable{
			auto readchan = client.ReadStartWithChannel();
			while (!stopfut.Ready()){
				auto readfut = readchan.Read();
				await(when_any(readfut, stopfut));
				if (stopfut.Ready()){
					break;
				}
				if (readfut.ErrorCode()){
					break;
				}
				auto buf = readfut.Get();
				client.Write(buf.Begin(), buf.Size());
				
			}
			client.ReadStop();

		});

		Uv::DefaultExecutor().Add(clientfunc);


	}


}

void uv_main( awaiter<void> await){
	// Tty correspoding to stdin, stdout, stderr
	Tty in{ 0, true };
	Tty out{ 1, false };
	Tty err{ 2, false };

	// Write out the prompt
	await(out.Write("Type Quit and press return to quit,\nFib to get the latest calculated fibonacci number,"
		"\nTimer <milliseconds> to wait milliseconds,"
		",Get <server> <resource> to get an http resource,"
		"\n any other text to have it echoed\n"));

	// Start the fibonacci writer

	// First make a channels to communicate with the fibonacci calculating function
	auto fibchan = make_channel < std::pair<std::uint16_t, std::uint64_t> >();
	auto stopchan = make_channel<int>();
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
		else if (s.substr(0, 5) == "Timer"){

			std::stringstream istr{ s };
			std::string dummy;
			std::uint64_t time{};
			istr >> dummy >> time;
			if (time == 0){
				await(out.Write("Please use a valid time in milliseconds that is greater than 0\n"));
			}else{
				Uv::DefaultExecutor().Add(std::bind(resumable<void>(timer), out, time));
			}
		}
		else if (s.substr(0, 3) == "Get"){
			std::string dummy, server, port, resource;
			std::stringstream istr{ s };
			istr >> dummy >> server >> port >> resource;
			if (server.empty() || port.empty() || resource.empty()){
				await(out.Write("Please enter a valid server, service and resource"));
			}
			resumable<std::string>(get_resource)(server,port, resource).Then(Uv::DefaultExecutor(), [out,server,port,resource](Future<std::string> f)mutable{
				auto ec = f.ErrorCode();
				if (ec < 0){
					std::stringstream s;
					s << "Get encountered error number " << ec << "While getting " << server << " " << port << " " << resource << "\n";
					out.Write(s.str());
				}
				out.Write(f.Get());

			});
		}
		else if (s.substr(0, 9) == "EchoStart"){
			std::stringstream istr{ s };
			std::string dummy;
			int port = 0;
			istr >> dummy >> port;
			if (port == 0){
				await(out.Write("Please enter a valid port\n"));
			}
			else{
				auto stopchan = make_channel<int>();
				Uv::DefaultExecutor().Add(std::bind(resumable<void>(tcp_echo_server), port, stopchan));
			}


		}
		else{
			await(out.Write("You typed " + s));
		}

	}
	// Stop reading
	in.ReadStop();

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

	// Run the executor loop
	Uv::DefaultExecutor().Loop();

}