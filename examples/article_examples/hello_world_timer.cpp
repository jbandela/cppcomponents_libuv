#include <cppcomponents_libuv/cppcomponents_libuv.hpp>

using cppcomponents::Future;

int main(){
    cppcomponents_libuv::Tty out{1,false};
    Future<int> output_fut = out.Write("Hello world...");
    output_fut.Then(
        [&](Future<int> f){
           cppcomponents_libuv::Timer::WaitFor(std::chrono::seconds{5}).Then([&](Future<int> f){
              out.Write("See you later").Then([](Future<int> f){ 
                  cppcomponents_libuv::Uv::DefaultExecutor().MakeLoopExit();
	      });
           });
        }
     );
    cppcomponents_libuv::Uv::DefaultExecutor().Loop();
}
