#include <sstream>

// Libuv wrapper
#include "../../cppcomponents_libuv/cppcomponents_libuv.hpp"

// For resumable and awaiter
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>


using namespace cppcomponents_libuv;
using namespace cppcomponents;


void handle_input(use<ITty> in, use<ITty> out, use<ITty> err, awaiter<void> await){
	await(out.Write("Type Quit and press return to quit, any other text to have it echoed\n"));

	auto chan = in.ReadStartWithChannel();

	while (true){


		auto buf = await(chan.Read());
		std::string s(buf.Begin(),buf.End());
		if (s.substr(0,4) == "Quit"){
			break;
		}
		else{
			await(out.Write("You typed " + s));
		}

	}

	in.ReadStop();

}

int uv_main(awaiter<int> await){

	// Exits the default executor when finished
	LoopExiter exiter;

	Tty in{ 0, true };
	Tty out{ 1, false };
	Tty err{ 1, false };

	// Dispatch our input handler
	auto quit_future = resumable<void>(handle_input)(in,out,err);

	//
	await(out.Write("Waiting 5 seconds before getting the url\n"));

	Timer timer;
	auto tc = timer.StartAsChannel(5000, 0);

	await(when_any(tc.Read(),quit_future));

	if (quit_future.Ready()){
		return 0;
	}

	// Look up the address asynchronously
	auto addr = await(Uv::Getaddrinfo(std::string("www.nodejs.org"), "http", nullptr));

	// If we cannot find address
	if (addr == nullptr){
		await(err.Write("Address not found"));
		return -1;
	}

	// Connect to the stream
	TcpStream stream;
	await(stream.Connect(addr->ai_addr));

	// Free the addr;
	Uv::Freeaddrinfo(addr);

	std::string request = "GET /about/ HTTP/1.0\r\nHost: www.nodejs.org\r\n\r\n";

	// Write the request
	await(stream.Write(request));


	// We get a channel instead of using a callback for reading
	auto chan = stream.ReadStartWithChannel();

	std::string response;

	bool more = true;
	while (true){

		// as_future allows us to get back a future that we can check the error code one
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
	auto wf = await.as_future(out.Write(response));
	if (wf.ErrorCode() < 0){
		std::stringstream sstream;
		sstream << "Write future returned " << wf.ErrorCode() << "\n";

		await(err.Write(sstream.str()));

	}
	// Wait unit we have typed quit
	await.as_future(quit_future);
	if (quit_future.ErrorCode() < 0){
		std::stringstream sstream;
		sstream << "Quit future returned " << quit_future.ErrorCode() << "\n";

		await(err.Write(sstream.str()));

	}
	

	// Return 0
	return 0;
}


int main(){

	resumable<int>(uv_main)();

	Uv::DefaultExecutor().Loop();




}