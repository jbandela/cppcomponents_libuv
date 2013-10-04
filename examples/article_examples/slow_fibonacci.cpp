#include <cppcomponents_libuv/cppcomponents_libuv.hpp>
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>
#include <iostream>
#include <sstream>
#include <thread>

using cppcomponents::Future;
using cppcomponents::Channel;

const std::uint16_t largest_64bit_fibonacci = 93;

void fibonacci(Channel < std::pair<std::uint16_t, std::uint64_t> > out_channel){
    auto write_future = out_channel.Write({ 0, 0 });
    std::uint64_t fibs[largest_64bit_fibonacci+1];
    fibs[0] = 0;
    fibs[1] = 1;
    for (std::uint16_t i = 2; i <= largest_64bit_fibonacci; ++i){
        fibs[i] = fibs[i - 2] + fibs[i - 1];
        if (write_future.Ready() || i==largest_64bit_fibonacci){
            write_future = out_channel.Write({ i, fibs[i] });
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void async_main(cppcomponents::awaiter await){
    cppcomponents_libuv::Tty out{1,false};
    cppcomponents_libuv::Tty in{0,true};
    auto input_channel = in.ReadStartWithChannel();
    await(out.Write("Enter Fib to see how far along the fibonacci calculation is\n"));
    cppcomponents_libuv::ThreadPoolExecutor pool;
    auto fib_channel =  cppcomponents::make_channel < std::pair<std::uint16_t, std::uint64_t> > ();
    pool.Add(std::bind(fibonacci,fib_channel));
    while(true){
	auto buf = await(input_channel.Read());
	std::string s{buf.Begin(),buf.End()};
	if(s.substr(0,3) == "Fib"){
	    std::uint16_t fib = 0;
    	    std::uint64_t value = 0;
	    std::tie(fib,value) = await(cppcomponents_libuv::Uv::DefaultExecutor(),fib_channel.Read());
	    std::stringstream os;
	    os << "Fibonacci(" << fib << ") = " << value << "\n";
	    await(out.Write(os.str()));
	    if(fib == largest_64bit_fibonacci){
		await(out.Write("Fibonacci calculations complete\n"));
		return;
	    }
	}
    }
}

int main(){
    auto resumable_main = cppcomponents::resumable(async_main);
    resumable_main().Then([&](Future<void> f){
	if(f.ErrorCode()){
	    std::cerr << "\nasync_main had error with error code " << f.ErrorCode() << "\n";
	}
	else{
	    std::cerr << "\nasync_main returned without throwing an exception\n";
	}
        cppcomponents_libuv::Uv::DefaultExecutor().MakeLoopExit();
	return f.ErrorCode();
    });
    cppcomponents_libuv::Uv::DefaultExecutor().Loop();
}
