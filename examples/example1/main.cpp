// libuv wrapper
#include <cppcomponents_libuv/cppcomponents_libuv.hpp>

// For resumable and awaiter
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>


// use<Interface> wraps a cppcomponents interface for use
// (the reason this is needed is that you can also implement_interface<Interface> when implementing an interface
using cppcomponents::use;

// Future<T> is just a template alias for use<IFuture<T>>
using cppcomponents::Future;

// Awaiter operator() uses boost::coroutine to emulate await on Future
using cppcomponents::awaiter;

// Resumable wraps a function taking awaiter into a function that returns Future
using cppcomponents::resumable;

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


void handle_input(use<ITty> in, use<ITty> out, use<ITty> err, awaiter<void> await){
	// Write out the prompt
	await(out.Write("Type Quit and press return to quit, any other text to have it echoed\n"));


	// Start reading and get it as a channel
	auto chan = in.ReadStartWithChannel();

	// Loop until we break
	while (true){
		// Read the channel and if we have Quit, the break otherwise echo
		auto buf = await(chan.Read());
		std::string s(buf.Begin(),buf.End());
		if (s.substr(0,4) == "Quit"){
			break;
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