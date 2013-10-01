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

#include <mutex>
#include <thread>
#include "../cppcomponents_libuv/cppcomponents_libuv.hpp"
//#define PPL_HELPER_OUTPUT_ENTER_EXIT
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

#include <gtest/gtest.h>



namespace luv = cppcomponents_libuv;
using cppcomponents::use;

const int TEST_PORT = 7171;

TEST(libuv_version, libuv_version){
	using namespace cppcomponents_libuv;

	auto s = Uv::VersionString();

	std::cerr << "Libuv version is " << s.to_string() << "\n";

}


TEST(async, async){
	int prepare_cb_called = 0;
	int async_cb_called = 0;
	int close_cb_called = 0;

	std::thread thread;
	std::mutex mutex;
	mutex.lock();

	luv::Loop loop{};
	luv::Prepare prepare{ loop };


	auto close_cb = [&](cppcomponents::Future<void>){
		close_cb_called++;
	};

	auto async_cb = [&](use<luv::IAsync> async, int status){
		EXPECT_EQ(status, 0);

		mutex.lock();
		int n = ++async_cb_called;
		mutex.unlock();

		if (n >= 3){
			async.Close().Then(close_cb);
			prepare.Close().Then(close_cb);
		}

	};
	luv::Async async{ loop, async_cb };


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
		EXPECT_EQ(status, 0);
		if (prepare_cb_called++)
			return;

		thread = std::thread(thread_cb);

		mutex.unlock();

	};

	prepare.Start(prepare_cb);

	loop.Run();

	EXPECT_GT(prepare_cb_called, 0);

	EXPECT_EQ(async_cb_called, 3);
	EXPECT_EQ(close_cb_called, 2);

	thread.join();





}


TEST(connection_fail, connection_fail){

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
		EXPECT_EQ(status, 0);
		timer_cb_calls++;

		EXPECT_EQ(close_cb_calls, 0);
		EXPECT_EQ(connect_cb_calls, 1);

		tcp.Close().Then(on_close);
		timer.Close().Then(timer_close_cb);
	};

	auto on_connect_with_close = [&](cppcomponents::Future<int> fut) {
		auto status = fut.ErrorCode();
		EXPECT_EQ(status, static_cast<int>(luv::ErrorCodes::Connrefused));
		connect_cb_calls++;

		EXPECT_EQ(close_cb_calls, 0);
		tcp.Close().Then(on_close);
	};

	auto on_connect_without_close = [&](cppcomponents::Future<int> fut){
		auto status = fut.ErrorCode();
		EXPECT_EQ(status, static_cast<int>(luv::ErrorCodes::Connrefused));
		connect_cb_calls++;
		timer.Start(timer_cb, std::chrono::milliseconds{ 100 }, std::chrono::milliseconds{ 0 });
		EXPECT_EQ(close_cb_calls, 0);
	};

	auto connection_fail =
		[&](std::function < void(cppcomponents::Future<int> fut)> connect_cb){

			auto client_addr = luv::Uv::Ip4Addr("0.0.0.0", 0);

			auto server_addr = luv::Uv::Ip4Addr("127.0.0.1", TEST_PORT);

			tcp = luv::TcpStream{ loop };

			tcp.Bind(client_addr);
			tcp.Connect(server_addr).Then(connect_cb);

			loop.Run();

			EXPECT_EQ(connect_cb_calls, 1);
			EXPECT_EQ(close_cb_calls, 1);

	};

	auto test_impl_connection_fail = [&](){
		connection_fail(on_connect_with_close);

		EXPECT_EQ(timer_close_cb_calls, 0);
		EXPECT_EQ(timer_cb_calls, 0);
	};

	auto test_impl_connection_fail_doesnt_auto_close = [&]() {

		timer = luv::Timer{ loop };

		connection_fail(on_connect_without_close);

		EXPECT_EQ(timer_close_cb_calls, 1);
		EXPECT_EQ(timer_cb_calls, 1);

	};

	test_impl_connection_fail();

	connect_cb_calls = 0;

	// Reset the counters
	close_cb_calls = 0;

	timer_close_cb_calls = 0;
	timer_cb_calls = 0;
	test_impl_connection_fail_doesnt_auto_close();


}

TEST(Tcp, TcpStream){
	using namespace cppcomponents_libuv;

	auto executor = Uv::DefaultExecutor();


	TcpStream server;

	auto server_addr = Uv::Ip4Addr("0.0.0.0", TEST_PORT);

	server.Bind(server_addr);



	server.Listen(1, cppcomponents::resumable([&](use<IUvStream> stream, int, cppcomponents::awaiter await){


		TcpStream client;
		stream.Accept(client);
		auto readchan = client.ReadStartWithChannel();

		int k = 0;
		while (true){
			auto fut = await.as_future(executor, readchan.Read());
			if (fut.ErrorCode()){
				break;
			}
			auto buf = fut.Get();
			std::string s{ buf.Begin(), buf.End() };
			EXPECT_EQ(s, "Hello");
			std::stringstream strstream;
			strstream << "Hi " << k++;
			std::string response = strstream.str();
			await(executor, client.Write(response));
		}
	}));


	auto client_func = cppcomponents::resumable([&](cppcomponents::awaiter await){
		for (int i = 0; i < 4; i++){
			auto client_address = Uv::Ip4Addr("127.0.0.1", TEST_PORT);
			TcpStream client;
			await(executor, client.Connect(client_address));
			auto chan = client.ReadStartWithChannel();
			await(executor, client.Write(std::string("Hello")));
			auto buf = await(executor, chan.Read());
			std::string response{ buf.Begin(), buf.End() };
			EXPECT_EQ(std::string("Hi 0"), response);
			await(client.Write(std::string("Hello")));
			buf = await(executor, chan.Read());
			response.assign(buf.Begin(), buf.End());
			EXPECT_EQ(std::string("Hi 1"), response);
		}


		server.Close();
		executor.MakeLoopExit();


	});

	client_func();
	executor.Loop();


}



#include <fcntl.h>
#include <sys/stat.h>

TEST(fs, fs1){
		using namespace cppcomponents_libuv;



	auto func = cppcomponents::resumable([&](cppcomponents::awaiter await){

		LoopExiter exiter;
		


		// Make a file
		std::string dir = "./testdir/";
		std::string filename = "./testdir/testfile.txt";

		// Create a directory
		EXPECT_EQ(0, await.as_future(Fs::Mkdir("./testdir", S_IWRITE | S_IREAD | S_IFDIR | S_IEXEC)).ErrorCode());

		// Create a file in the directory
		auto openfut = await.as_future(Fs::Open(filename, O_CREAT | O_RDWR, S_IWRITE | S_IREAD));
		EXPECT_EQ(0, openfut.ErrorCode());

		auto file = await(openfut);

		// Write some data
		std::string out = "Hello World";
		EXPECT_EQ(0, await.as_future(Fs::Write(file,out.data(), out.size(), 0)).ErrorCode());

		// Sync and close
		EXPECT_EQ(0, await.as_future(Fs::Sync(file)).ErrorCode());
		EXPECT_EQ(0, await.as_future(Fs::Close(file)).ErrorCode());

		// Open it again
		openfut = await.as_future(Fs::Open(filename, O_RDONLY, 0));
		EXPECT_EQ(0, openfut.ErrorCode());
		file = await(openfut);

		std::vector<char> buf(100);

		// Read from it
		auto sz = await(Fs::Read(file,&buf[0], buf.size(), 0));
		std::string in{ buf.begin(), buf.begin() + sz };
		EXPECT_EQ(out, in);


		EXPECT_EQ(0, await.as_future(Fs::Close(file)).ErrorCode());

		openfut = await.as_future(Fs::Open(filename, O_WRONLY, 0));

		EXPECT_EQ(0, openfut.ErrorCode());

		file = await(openfut);

		// Truncate it
		EXPECT_EQ(0, await.as_future(Fs::Truncate(file,5)).ErrorCode());

		// Close it
		EXPECT_EQ(0, await.as_future(Fs::Close(file)).ErrorCode());

		// Open it again
		openfut = await.as_future(Fs::Open(filename, O_RDONLY, 0));
		EXPECT_EQ(0,openfut.ErrorCode());

		file = await(openfut);

		// Check out truncated read
		sz = await(Fs::Read(file,&buf[0], buf.size(), 0));
		in.assign(buf.begin(), buf.begin() + sz);
		EXPECT_EQ(in, "Hello");
		EXPECT_EQ(0, await.as_future(Fs::Close(file)).ErrorCode());

		// Stat it
		auto stat = await(Fs::Stat(filename));
		EXPECT_EQ(stat.st_size, 5);

		// Check that it is in the directory
		auto dirfiles = await(Fs::Readdir(dir, 0));
		EXPECT_EQ(dirfiles.size(), 1);
		EXPECT_EQ(dirfiles[0], "testfile.txt");

		// Delete the file
		EXPECT_EQ(0, await.as_future(Fs::Unlink(filename)).ErrorCode());

		// Check that it is not in the directory
		dirfiles = await(Fs::Readdir(dir, 0));
		EXPECT_EQ(dirfiles.size(), 0);

		// Check that the directory we created is present
		dirfiles = await(Fs::Readdir("./", 0));
		auto iter = std::find(dirfiles.begin(), dirfiles.end(), std::string{ "testdir" });
		EXPECT_NE(iter, dirfiles.end());

		// Delete the directory
		EXPECT_EQ(0, await.as_future(Fs::Rmdir(dir)).ErrorCode());

		// Check that it is no longer present
		dirfiles = await(Fs::Readdir("./", 0));
		iter = std::find(dirfiles.begin(), dirfiles.end(), std::string{ "testdir" });
		EXPECT_EQ(iter, dirfiles.end());

	});

	auto fut = func();

	Uv::DefaultExecutor().Loop();

	fut.Get();

}

TEST(udp, udp){
	using namespace cppcomponents_libuv;
	
	std::string result;


	luv::UdpStream send_socket;
	luv::UdpStream recv_socket;

	auto func = cppcomponents::resumable([&](cppcomponents::awaiter await){
		LoopExiter exiter;

		auto recv_addr = luv::Uv::Ip4Addr("0.0.0.0", 7768);
		recv_socket.Bind(recv_addr, 0);
		recv_socket.RecvStart([&](use<IUdpStream> stream, std::intptr_t nread,
			cppcomponents::use<cppcomponents::IBuffer> buf, luv::SockAddr addr, unsigned int flags){
				std::string s{ buf.Begin(), buf.End() };
				EXPECT_EQ(s, "Hello UDP");
				stream.RecvStop();
				result = s;
		});

		send_socket.Bind(Uv::Ip4Addr("0.0.0.0", 0), 0);
		send_socket.SetBroadcast(1);

		auto send_addr = Uv::Ip4Addr("255.255.255.255", 7768);
		EXPECT_EQ(0, await.as_future(send_socket.Send(std::string("Hello UDP"), send_addr)).ErrorCode());



	});

	auto fut = func();

	Uv::DefaultExecutor().Loop();
	EXPECT_EQ(0, fut.ErrorCode());
	EXPECT_EQ(result, "Hello UDP");

}


TEST(async2, async2){

	using namespace cppcomponents_libuv;



	auto calculate = cppcomponents::resumable([&](cppcomponents::awaiter await){

		auto f1 = Uv::Async([](){return 7; });

		auto f2 = Uv::Async([](){return 3; });

		return await(f1) * await(f2);


	});

	auto func = cppcomponents::resumable([&](cppcomponents::awaiter await){
		LoopExiter exiter;

		int value = await(calculate());

		EXPECT_EQ(21, value);


	});

	func();

	Uv::DefaultExecutor().Loop();

}