#include <cppcomponents_libuv/cppcomponents_libuv.hpp>
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>
#include <iostream>
#include <sstream>
#include <thread>

using cppcomponents::Future;
using cppcomponents::Channel;

std::string get_resource(const std::string& server,const std::string& port, const std::string& resource, 
        cppcomponents::awaiter await){

    auto addr = await(cppcomponents_libuv::Uv::Getaddrinfo(server, port, nullptr));
    if (addr == nullptr){
        return "ERROR: Address not found\n";
    }
    cppcomponents_libuv::TcpStream stream;
    await(stream.Connect(addr->ai_addr));
    cppcomponents_libuv::Uv::Freeaddrinfo(addr);
    std::string request = "GET " + resource + " HTTP/1.0\r\nHost: " + server + "\r\n\r\n";
    await(stream.Write(request));
    auto chan = stream.ReadStartWithChannel();
    std::string response;
    while (true){
        auto timerfut = cppcomponents_libuv::Timer::WaitFor(std::chrono::seconds{ 2 });
        auto fut = chan.Read();
        await(when_any(timerfut, fut));
        if (timerfut.Ready()){
            return "Error " + server + " timed out while reading\n";
        }   
        if (fut.ErrorCode() != 0){
            if(fut.ErrorCode() == static_cast<int>(cppcomponents_libuv::ErrorCodes::Eof)){
               return response;
            }
            return "Error " + server + " return an error while reading\n";
        }
        auto buf = fut.Get();
        if(buf.Size() == 0){
            return response;
        }
        response.append(buf.Begin(), buf.End());
    }
}

void async_main(cppcomponents::awaiter await){
    cppcomponents_libuv::Tty out{1,false};
    cppcomponents_libuv::Tty in{0,true};
    auto input_channel = in.ReadStartWithChannel();
    await(out.Write("Enter <server> <port> <resource> to get a http resource\n"));
    auto buf = await(input_channel.Read());
    std::string s{buf.Begin(),buf.End()};
    std::stringstream is{s};
    std::string server, port, resource;
    is >> server >> port >> resource;
    auto resumable_get_resource = cppcomponents::resumable(get_resource);
    auto result = await(resumable_get_resource(server,port,resource));
    await(out.Write(result));
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
