#include <cppcomponents_libuv/cppcomponents_libuv.hpp>

using cppcomponents::Future;

int main(){
    cppcomponents_libuv::Tty out{1,false};
    Future<int> output_fut = out.Write("Hello world\n");
    output_fut.Then(
        [](Future<int> f){
            cppcomponents_libuv::Uv::DefaultExecutor().MakeLoopExit();
        }
     );
    cppcomponents_libuv::Uv::DefaultExecutor().Loop();
}