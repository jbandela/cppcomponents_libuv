#include <cppcomponents_libuv/cppcomponents_libuv.hpp>
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

using cppcomponents::Future;

void async_main(cppcomponents::awaiter await){

    cppcomponents_libuv::Tty out{1,false};
    cppcomponents_libuv::Tty in{0,true};
    auto input_channel = in.ReadStartWithChannel();
    await(out.Write("Please enter your name in the next 5 seconds \n"));
    auto timeout_future = cppcomponents_libuv::Timer::WaitFor(std::chrono::seconds{5});
    auto input_future = input_channel.Read();
    await(cppcomponents::when_any(timeout_future,input_future));
    if(timeout_future.Ready()){
	await(out.Write("You did not answer fast enough. Goodbye.\n"));
    	cppcomponents_libuv::Uv::DefaultExecutor().MakeLoopExit();
	return;
    }
    auto buffer = input_future.Get();
    await(out.Write("Hello " + std::string(buffer.Begin(),buffer.End()) + " nice to meet you..."));
    await(cppcomponents_libuv::Timer::WaitFor(std::chrono::seconds{5}));
    await( out.Write("See you later"));
    cppcomponents_libuv::Uv::DefaultExecutor().MakeLoopExit();
}

int main(){
    auto resumable_main = cppcomponents::resumable(async_main);
    resumable_main();
    cppcomponents_libuv::Uv::DefaultExecutor().Loop();
}
