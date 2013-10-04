#include <cppcomponents_libuv/cppcomponents_libuv.hpp>
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

using cppcomponents::Future;

void async_main(cppcomponents::awaiter await){

    cppcomponents_libuv::Tty out{1,false};
    await(out.Write("Hello world..."));
    await(cppcomponents_libuv::Timer::WaitFor(std::chrono::seconds{5}));
    await( out.Write("See you later"));
    cppcomponents_libuv::Uv::DefaultExecutor().MakeLoopExit();
}

int main(){
    auto resumable_main = cppcomponents::resumable(async_main);
    resumable_main();
    cppcomponents_libuv::Uv::DefaultExecutor().Loop();
}
