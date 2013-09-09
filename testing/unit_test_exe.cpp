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

	auto loop = luv::Loop::DefaultLoop();

	use<luv::ITcpStream> server = luv::TcpStream { loop }.QueryInterface<luv::ITcpStream>();

	struct sockaddr_in bind_addr = luv::Uv::Ip4Addr("0.0.0.0", 7000);

	server.Bind(bind_addr);
		int counter = 0;

	server.Listen(1,cppcomponents::resumable<void>([&](use<luv::IStream> s, int status,cppcomponents::awaiter<void> await)mutable{
		luv::TcpStream client(luv::Loop::DefaultLoop());
		s.Accept(client.QueryInterface<luv::IStream>());
		auto chan = client.GetReadChannel();

		auto buf = await(chan.Read());

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
			client.Write(response);

	


			cross_compiler_interface::module m("cppcomponents_libuv_dll");
			auto get_object_count = m.load_module_function<std::size_t(*)()>("GetObjectCount");
			auto count = get_object_count();
			std::cerr << "\nObject count = " << count << "\n\n";


	}));

	loop.Run();
	return 0;

}