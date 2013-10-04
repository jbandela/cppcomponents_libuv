#include <cppcomponents_libuv/cppcomponents_libuv.hpp>
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>
#include <iostream>
#include <sstream>
#include <thread>

using cppcomponents::Future;
using cppcomponents::Channel;

void echo_server(int port, Channel<int> stopchan, cppcomponents::awaiter await){

	auto stopfut = stopchan.Read();

    cppcomponents_libuv::TcpStream server;
	auto server_addr = cppcomponents_libuv::Uv::Ip4Addr("0.0.0.0", port);
	server.Bind(server_addr);

	server.Listen(1, cppcomponents::resumable([](cppcomponents::use<cppcomponents_libuv::IUvStream> is,
                    int, cppcomponents::awaiter await){

		cppcomponents_libuv::TcpStream client;
		is.Accept(client);
		auto readchan = client.ReadStartWithChannel();
        while ( true ){
            auto buf = await(readchan.Read());

            std::stringstream strstream;
            strstream <<
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " << buf.Size() << "\r\n"
            "\r\n";

            strstream.write(buf.Begin(), buf.Size());

            await(client.Write(strstream.str()));

        }
    }));
    await(stopfut);
}

void async_main(cppcomponents::awaiter await){
    cppcomponents_libuv::Tty out{1,false};
    cppcomponents_libuv::Tty in{0,true};
    auto input_channel = in.ReadStartWithChannel();
    await(out.Write("Enter <port> to start an echo server\n"));
    auto buf = await(input_channel.Read());
    std::string s{buf.Begin(),buf.End()};
    std::stringstream is{s};
    int port = -1;
    is >> port;
    if(port==-1){
        await(out.Write("Invalid port entered, exiting\n"));
        return;
    }
    auto server_chan = cppcomponents::make_channel<int>();
    auto resumable_echo_server = cppcomponents::resumable(echo_server);
    auto server_fut = resumable_echo_server(port,server_chan);
    auto read_fut = input_channel.Read();
    await(out.Write("Starting server, enter Quit to stop server\n"));
    await(when_any(server_fut,read_fut));
    if(server_fut.Ready()){
        await(out.Write("Error starting server, exiting\n"));
        server_fut.Get();
    }
    while(true){
        buf = await(read_fut);
        s.assign(buf.Begin(),buf.End());
        if(s.substr(0,4) == "Quit"){
            server_chan.Write(1);
            await(server_fut);
            return;
        }
        read_fut = input_channel.Read();
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
