#include <cppcomponents_libuv/cppcomponents_libuv.hpp>



int main(){
    auto exiter = [](){cppcomponents_libuv::Uv::DefaultExecutor().MakeLoopExit();};
    cppcomponents_libuv::Uv::DefaultExecutor().Add(exiter);
    cppcomponents_libuv::Uv::DefaultExecutor().Loop();
}