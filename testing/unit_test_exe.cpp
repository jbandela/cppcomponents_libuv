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

	luv::TcpStream server{ loop };

	struct sockaddr_in bind_addr = luv::Uv::Ip4Addr("0.0.0.0", 7000);

	server.Bind(bind_addr);

	server.Listen(1,cppcomponents::resumable<void>([](use<luv::IStream> s, int status, cppcomponents::awaiter<void> await){

		luv::TcpStream client(luv::Loop::DefaultLoop());
		s.Accept(client.QueryInterface<luv::IStream>());

		auto chan = client.GetReadChannel();
		while (true){

		
			auto vec = await(chan.Read());
			std::string str{ vec.begin(), vec.end() };
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
			await(client.Write(response));
		}


	}));

	loop.Run();

	return 0;

}