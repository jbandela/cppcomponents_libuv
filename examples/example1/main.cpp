#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include <sstream>
namespace{
	struct MemLeakCheckInit{
		MemLeakCheckInit(){
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			//_crtBreakAlloc = 579;
		}
	};

	MemLeakCheckInit mlcinit;
}

#endif


// Libuv wrapper
#include "../../cppcomponents_libuv/cppcomponents_libuv.hpp"

// For resumable and awaiter
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

#include <iostream>

using namespace cppcomponents_libuv;
using namespace cppcomponents;


void handle_input(awaiter<void> await){
	std::cerr << "Type Quit and press return to quit, any other text to have it echoed\n";
	while (true){
		std::vector<char> vec(1024);

		auto sz = await(Fs::Read(0, &vec[0], vec.size(), 0));
		std::string s(vec.begin(), vec.begin() + sz);
		if (s.substr(0,4) == "Quit"){
			break;
		}
		else{
			std::cerr << "You typed " << s;
		}

	}

}

template<class T>
Future<T> fork_future(Future<T>& f){

	auto p1 = make_promise<T>();
	auto p2 = make_promise<T>();

	auto f1 = f;
	f1.Then([p1, p2](Future<T> f){

		try{
			p1.Set(f.Get());
			p2.Set(f.Get());
		}
		catch (std::exception& e){
			p1.SetError(cross_compiler_interface::general_error_mapper::error_code_from_exception(e));
			p2.SetError(cross_compiler_interface::general_error_mapper::error_code_from_exception(e));
		}
	});

	f = p1.QueryInterface<IFuture<T>>();
	return p2.QueryInterface<IFuture<T>>();
}
inline Future<void> fork_future(Future<void>& f){

	auto p1 = make_promise<void>();
	auto p2 = make_promise<void>();

	auto f1 = f;
	f1.Then([p1, p2](Future<void> f){

		try{
			f.Get();
			p1.Set();

			p2.Set();
		}
		catch (std::exception& e){
			p1.SetError(cross_compiler_interface::general_error_mapper::error_code_from_exception(e));
			p2.SetError(cross_compiler_interface::general_error_mapper::error_code_from_exception(e));
		}
	});

	f = p1.QueryInterface<IFuture<void>>();
	return p2.QueryInterface<IFuture<void>>();
}

int uv_main(awaiter<int> await){

	// Exits the default executor when finished
	LoopExiter exiter;

	// Dispatch our input handler
	auto quit_future = resumable<void>(handle_input)();

	//
	std::cerr << "Waiting 5 seconds before getting the url\n";

	Timer timer;
	auto tc = timer.StartAsChannel(5000, 0);

	await(when_any(tc.Read(),fork_future(quit_future)));

	if (quit_future.Ready()){
		return 0;
	}

	// Look up the address asynchronously
	auto addr = await(Uv::Getaddrinfo(std::string("www.nodejs.org"), "http", nullptr));

	// If we cannot find address
	if (addr == nullptr){
		std::cerr << "Address not found";
		return -1;
	}

	// Connect to the stream
	TcpStream stream;
	await(stream.Connect(addr->ai_addr));

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
	std::cout << response;

	// Wait unit we have typed quit
	await(quit_future);

	// Return 0
	return 0;
}


int main(){

	resumable<int>(uv_main)();

	Uv::DefaultExecutor().Loop();




}