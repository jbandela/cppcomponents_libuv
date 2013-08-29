#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Psapi.lib")


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

namespace luv = cppcomponents_libuv;
using cppcomponents::use;
using cppcomponents::make_delegate;
int64_t counter = 0;

	void wait_for_a_while(use<luv::IIdle> handle, int status) {
		counter++;

		if (counter >= 1e6)
			handle.Stop();
	}


int main() {

	auto loop = luv::Loop::DefaultLoop();

	luv::Idle idler{loop};

	idler.StartRaw(make_delegate<luv::IdleCallback>(wait_for_a_while));



	printf("Idling...\n");
	loop.Run();

	return 0;

}