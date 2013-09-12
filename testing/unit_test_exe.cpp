#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>

namespace{
	struct MemLeakCheckInit{
		MemLeakCheckInit(){
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
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
	cppcomponents::LoopExecutor executor;

	//auto loop = luv::Loop::DefaultLoop();
	luv::Loop loop;

	luv::TcpStream server{ loop };

	luv::Prepare prep{ loop };
	prep.Start([&](cppcomponents::use<luv::IPrepare>, int status){
		executor.RunQueuedClosures();
	});

	struct sockaddr_in bind_addr = luv::Uv::Ip4Addr("0.0.0.0", 7000);

	server.Bind(bind_addr);
	int counter = 0;

	server.Listen(1, [&loop, &executor](use<luv::IStream> s, int status){
		assert(_CrtCheckMemory());
		luv::TcpStream clientstream(loop.as<luv::ILoop>());
		auto client = clientstream.as<luv::IStream>();
		s.Accept(client);
		auto uchan = client.GetReadChannel();
		use < cppcomponents::IChannel < std::vector < char >> > chan = uchan.get();
		uchan.release();
		chan.Read().Then(executor.QueryInterface<cppcomponents::IExecutor>(), [&executor, client](cppcomponents::use < cppcomponents::IFuture < std::vector<char >> > f)mutable{
			auto buf = f.Get();
			assert(_CrtCheckMemory());

			std::string str{ buf.begin(), buf.end() };
			std::cerr << str;
			std::string response = R"(HTTP/1.1 200 OK
Date: Fri, 31 Dec 1999 23:59:59 GMT
Content-Type: text/plain
Content-Length: 42
Connection: Close
some-footer: some-value
another-footer: another-value

abcdefghijklmnopqrstuvwxyz1234567890abcdef
)";
			assert(_CrtCheckMemory());
			client.Write(response).Then(executor, [client](cppcomponents::use < cppcomponents::IFuture < int >> f){
				int i = f.Get();
			});
			assert(_CrtCheckMemory());



			cross_compiler_interface::module m("cppcomponents_libuv_dll");
			auto get_object_count = m.load_module_function<std::size_t(*)()>("GetObjectCount");
			auto count = get_object_count();
			std::cerr << "\nObject count = " << count << "\n\n";

		});


	});

	loop.Run();
	return 0;

}