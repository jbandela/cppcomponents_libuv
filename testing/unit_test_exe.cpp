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

	auto loop = luv::Loop::DefaultLoop();

	luv::TcpStream server{ loop };

	luv::Prepare prep{ loop };
	prep.Start([&](cppcomponents::use<luv::IPrepare>, int status){
		executor.RunQueuedClosures();
	});

	struct sockaddr_in bind_addr = luv::Uv::Ip4Addr("0.0.0.0", 7001);

	server.Bind(bind_addr);
	int counter = 0;

	server.Listen(1, cppcomponents::resumable<void>([&loop,&executor](use<luv::IStream> s, int status, cppcomponents::awaiter<void> await){
		luv::TcpStream client{loop};
		s.Accept(client);
		auto chan = client.GetReadChannel();


		bool has_more = true;
		std::string data;
		while (has_more){
			auto f = await.as_future(executor,chan.Read());
			auto errcode = f.ErrorCode();
			if (errcode < 0){
				has_more = false;
			}
			else{
				auto buf = f.Get();
				data.insert(data.end(),buf.Begin(), buf.End());
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
		}
		std::cerr << data;

			cross_compiler_interface::module m("cppcomponents_libuv_dll");
			auto get_object_count = m.load_module_function<std::size_t(*)()>("GetObjectCount");
			auto count = get_object_count();
			std::cerr << "\nObject count = " << count << "\n\n";








	}));

	loop.Run();
	return 0;

}