#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#define PPL_HELPER_OUTPUT_ENTER_EXIT
namespace{
	struct MemLeakCheckInit{
		MemLeakCheckInit(){
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			_crtBreakAlloc = 1912;
		}
	};

	MemLeakCheckInit mlcinit;
}

#endif
#include <stdio.h>
#include <stdlib.h>
#include <atomic>
#include <type_traits>
#include "../cppcomponents_libuv/cppcomponents_libuv.hpp"
#include <memory>
#include <sstream>
#include <string>
#include <iostream>
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

namespace luv = cppcomponents_libuv;
using cppcomponents::use;




int main() {
	{
		auto loop = luv::Loop::DefaultLoop();

		cppcomponents::LoopExecutor executor;
		luv::Prepare prep{loop};

		prep.Start([&executor](use<luv::IPrepare>, int status){

			executor.RunQueuedClosures();
		});




	luv::TcpStream server{ loop };

	auto bind_addr = luv::Uv::Ip4Addr("0.0.0.0", 7001);

	server.Bind(bind_addr);

	server.Listen(1, cppcomponents::resumable<void>([&](use<luv::IStream> s, int status, cppcomponents::awaiter<void> await){
		luv::TcpStream client{loop};
		s.Accept(client);
		auto chan = client.GetReadChannel();
				cppcomponents::unique_channel<cppcomponents::use<cppcomponents::IBuffer>> uchan{chan};


		std::string data;
		while (true){
			auto f = await.as_future(executor,chan.Read());
			auto errcode = f.ErrorCode();
			if (errcode < 0){
				break;
			}

			auto buf = f.Get();
			data.insert(data.end(), buf.Begin(), buf.End());

			std::string response = R"(HTTP/1.1 200 OK
Date: Fri, 31 Dec 1999 23:59:59 GMT
Content-Type: text/plain
Content-Length: 42
Connection: Close
some-footer: some-value
another-footer: another-value

abcdefghijklmnopqrstuvwxyz1234567890abcdef
)";
			await(executor,client.Write(response));
			
		}
		chan.Close();
		std::cerr << data;

		// Output the number of live libuv objects
		cross_compiler_interface::module m("cppcomponents_libuv_dll");
		auto get_object_count = m.load_module_function<std::size_t(*)()>("GetObjectCount");
		auto count = get_object_count();
		std::cerr << "\nObject count = " << count << "\n\n";
		await(executor,server.Close());

		loop.Stop();
	}));


	auto clientfunc = cppcomponents::resumable<void>([&](cppcomponents::awaiter<void> await){
			auto bind_addr = luv::Uv::Ip4Addr("127.0.0.1", 7001);
	luv::TcpStream client{loop};

		await(executor,client.Connect(bind_addr));
		auto reader = client.GetReadChannel();
		cppcomponents::unique_channel<cppcomponents::use<cppcomponents::IBuffer>> uchan{reader};
		for (int i = 0; i < 15; i++){
			await(executor,client.Write(cppcomponents::cr_string("Hello")));
			auto buf = await(reader.Read());
			std::string s{ buf.Begin(), buf.End() };

		}

		reader.Close();
		await(executor,client.Close());


	});

	clientfunc();
	//clientfunc();
	loop.Run();
		loop.RunNoWait();
	while (executor.NumPendingClosures()){
		loop.RunNoWait();
	}
	prep.Stop();
	}
	return 0;

}