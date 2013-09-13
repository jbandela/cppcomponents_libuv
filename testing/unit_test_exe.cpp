#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
namespace{
	struct MemLeakCheckInit{
		MemLeakCheckInit(){
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			//_crtBreakAlloc = 1284;
		}
	};

	MemLeakCheckInit mlcinit;
}

#endif

#include <mutex>
#include <thread>
#include "../cppcomponents_libuv/cppcomponents_libuv.hpp"
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

#define CATCH_CONFIG_MAIN  
#include "external/catch.hpp"

namespace luv = cppcomponents_libuv;
using cppcomponents::use;

const int TEST_PORT = 7171;


TEST_CASE("test-async", "test-async"){
		int prepare_cb_called = 0;
	int async_cb_called = 0;
	int close_cb_called = 0;

	std::thread thread;
	std::mutex mutex;
	mutex.lock();

	auto loop = luv::Loop::DefaultLoop();
	luv::Prepare prepare{ loop };


	auto close_cb = [&](cppcomponents::Future<void>){
		close_cb_called++;
	};

	auto async_cb = [&](use<luv::IAsync> async, int status){
		REQUIRE(status == 0);

		mutex.lock();
		int n = ++async_cb_called;
		mutex.unlock();

		if (n >= 3){
			async.Close().Then(close_cb);
			prepare.Close().Then(close_cb);
		}

	};
	luv::Async async{loop,async_cb};


	auto thread_cb = [&](){
		int n;
		for (;;){
			mutex.lock();
			n = async_cb_called;
			mutex.unlock();

			if (n == 3){
				break;
			}
			async.Send();

			std::this_thread::sleep_for(std::chrono::milliseconds(0));

		}

	};





	auto prepare_cb = [&](use<luv::IPrepare> p, int status){
		REQUIRE(status == 0);
		if (prepare_cb_called++)
			return;

		thread = std::thread(thread_cb);

		mutex.unlock(); 

	};

	prepare.Start(prepare_cb);

	loop.Run();

	REQUIRE(prepare_cb_called > 0);

	REQUIRE(async_cb_called == 3);
	REQUIRE(close_cb_called == 2);

	thread.join();



	


}


TEST_CASE("test-connection-fail", "test-connection-fail"){

	auto loop = luv::Loop::DefaultLoop();
	use<luv::ITcpStream> tcp;
	//static uv_connect_t req;
	int connect_cb_calls = 0;
	int close_cb_calls = 0;

	use<luv::ITimer> timer;
	int timer_close_cb_calls = 0;
	int timer_cb_calls = 0;

	auto on_close = [&](cppcomponents::Future<void>){
		close_cb_calls++;
	};

	auto timer_close_cb = [&](cppcomponents::Future<void>){
		timer_close_cb_calls++;
	};

	auto timer_cb = [&](use<luv::ITimer>, int status){
		REQUIRE(status == 0);
		timer_cb_calls++;

		REQUIRE(close_cb_calls == 0);
		REQUIRE(connect_cb_calls == 1);

		tcp.Close().Then(on_close);
		timer.Close().Then(timer_close_cb);
	};

	auto on_connect_with_close = [&](cppcomponents::Future<int> fut) {
		auto status = fut.ErrorCode();
		REQUIRE(status == luv::ErrorCodes::Connrefused);
		connect_cb_calls++;

		REQUIRE(close_cb_calls == 0);
		tcp.Close().Then(on_close);
	};

	auto on_connect_without_close = [&](cppcomponents::Future<int> fut){
		auto status = fut.ErrorCode();
		REQUIRE(status == luv::ErrorCodes::Connrefused);
		connect_cb_calls++;
		timer.Start(timer_cb, 100, 0);
		REQUIRE(close_cb_calls == 0);
	};

	auto connection_fail = 
		[&](std::function < void(cppcomponents::Future<int> fut)> connect_cb){

			auto client_addr = luv::Uv::Ip4Addr("0.0.0.0", 0);
			
			auto server_addr = luv::Uv::Ip4Addr("127.0.0.1", TEST_PORT);

			tcp = luv::TcpStream{ loop };

			tcp.Bind(client_addr);
			tcp.Connect(server_addr).Then(connect_cb);

			loop.Run();

			REQUIRE(connect_cb_calls == 1);
			REQUIRE(close_cb_calls == 1);

	};

	auto test_impl_connection_fail = [&](){
		connection_fail(on_connect_with_close);

		REQUIRE(timer_close_cb_calls == 0);
		REQUIRE(timer_cb_calls == 0);
	};

	auto test_impl_connection_fail_doesnt_auto_close = [&]() {

		timer = luv::Timer{ loop };

		connection_fail(on_connect_without_close);

		REQUIRE(timer_close_cb_calls == 1);
		REQUIRE(timer_cb_calls == 1);

	};

	test_impl_connection_fail();

	connect_cb_calls = 0;

	// Reset the counters
	close_cb_calls = 0;

	timer_close_cb_calls = 0;
	timer_cb_calls = 0;
	test_impl_connection_fail_doesnt_auto_close();

}

TEST_CASE("Listener_read", "Listener_reader"){

		auto loop = luv::Loop::DefaultLoop();

	auto f = cppcomponents::resumable<void>([&](cppcomponents::awaiter<void> await){
		use<luv::ITcpStream> server = luv::TcpStream{ loop };
		auto server_addr = luv::Uv::Ip4Addr("127.0.0.1", TEST_PORT);

		server.Bind(server_addr);
		auto chan = server.GetListenChannel(1);

		for (int i = 0; i < 2; i++){
			auto fut = await.as_future(chan.Read());
			if (fut.ErrorCode()) break;
			auto stream = fut.Get();
			luv::TcpStream client{ loop };
			stream.Accept(client);
			auto readchan = client.GetReadChannel();

			while (true){
				auto fut = await.as_future(readchan.Read());
				if (fut.ErrorCode()) break;
				auto buf = fut.Get();
				std::string s{ buf.Begin(), buf.End() };
				std::cerr << s;
				std::string response =R"(HTTP/1.1 200 OK
Date: Fri, 31 Dec 1999 23:59:59 GMT
Content-Type: text/plain
Content-Length: 42
some-footer: some-value
another-footer: another-value

abcdefghijklmnopqrstuvwxyz1234567890abcdef)";
				await(client.Write(response));
			}

		}
		server.Close();

	});

	f();
	loop.Run();


	

}