// libuv wrapper
#include <cppcomponents_libuv/cppcomponents_libuv.hpp>

// For resumable and awaiter
#include <cppcomponents_async_coroutine_wrapper/cppcomponents_resumable_await.hpp>

// For stringstream
#include <sstream>

// For this_thread and sleep_for
#include <thread>
#include <chrono>

#include <map>

// use<Interface> wraps a cppcomponents interface for use
// (the reason this is needed is that you can also implement_interface<Interface> when implementing an interface)
using cppcomponents::use;

// Future<T> is just a template alias for use<IFuture<T>>
using cppcomponents::Future;

// Awaiter operator() uses boost::coroutine to emulate await on Future
using cppcomponents::awaiter;

// Resumable wraps a function taking awaiter into a function that returns Future
using cppcomponents::resumable;

// Like a Go channel (a little bit)
// A template alias for Channel<T> = use<IChannel<T>>
using cppcomponents::Channel;

// Create a channel
using cppcomponents::make_channel;

//

// libuv types, Interfaces start with I
// Classes do not start with I and provide implementation of interfaces
using cppcomponents_libuv::ITty;
using cppcomponents_libuv::Tty;
using cppcomponents_libuv::TcpStream;
using cppcomponents_libuv::Timer;
using cppcomponents_libuv::IUvStream;

// The general purpose libuv global methods are static methods under Uv
using cppcomponents_libuv::Uv;

// Use RAII to guarantee call to Uv::DefaultExecutor().MakeLoopExit();
using cppcomponents_libuv::LoopExiter;

using cppcomponents_libuv::ErrorCodes;


void fibonacci(std::uint16_t n, Channel < std::pair<std::uint16_t, std::uint64_t> > chan, Channel<int> stopchan){
	// First fibonacci is 0
	// So go ahead and write that as our first value
	auto fut = chan.Write({ 0, 0 });

	auto stopfut = stopchan.Read();

	// A vector to hold our intermediate calculations
	std::vector<std::uint64_t> fibs(n+1);

	
	if (n == 0){
		return;
	}
	if (n == 1){
		chan.Write({ 1, 1 });
		return;
	}

	// Set the 0th and 1st fibonacci numbers
	fibs[0] = 0;
	fibs[1] = 1;

	// Calculate all the fibonacci numbers up to and including n
	for (std::uint16_t i = 2; i <= n; ++i){
		// Fibonacci forumula
		fibs[i] = fibs[i - 2] + fibs[i - 1];

		// fut (the Future from chan.Write, will become ready when somebody reads the channel
		// that is a trigger that we need to write the latest value to the channel
		// Also, if we are done calculating (i==n) we write the value to the channel
		if (fut.Ready() || i==n || stopfut.Ready()){
			fut = chan.Write({ i, fibs[i] });

			// Check if we were called to stop
			if (stopfut.Ready()){
				return;
			}
		}
		
		
		// Slow us down so it looks like a long calculation, otherwise it runs too fast
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	

}

void timer(use<ITty> out, std::chrono::milliseconds time, awaiter await){


	await(Timer::WaitFor(time));
	std::stringstream s;
	s << "Finished waiting " << (double)(time.count()) / 1000.0 << "seconds \n";
	out.Write(s.str());
}


std::string get_resource(const std::string& server,const std::string& port, const std::string& resource, awaiter await){

	// Look up the address asynchronously
	auto addr = await(Uv::Getaddrinfo(server, port, nullptr));

	// If we cannot find address
	if (addr == nullptr){
		return "ERROR: Address not found\n";
	}

	// Connect to the stream
	TcpStream stream;
	await(stream.Connect(addr->ai_addr));

	// Free the addr now that we no longer need it
	Uv::Freeaddrinfo(addr);

	// Send http get request to www.nodejs.org/about/
	std::string request = "GET " + resource + " HTTP/1.0\r\nHost: " + server + "\r\n\r\n";

	await(stream.Write(request));


	// We get a channel instead of using a callback for reading
	auto chan = stream.ReadStartWithChannel();

	// This holds the response we get back from the server
	std::string response;

	while (true){

		// as_future allows us to get back a future that we can check the error code instead of throwing exception 
		// put a timeout of 2 seconds on the read
		auto timerfut = Timer::WaitFor(std::chrono::seconds{ 2 });
		auto fut = chan.Read();

		// Await for either a read or timeout
		await(when_any(timerfut, fut));

		// If we time out then break
		if (timerfut.Ready()){
			break;
		}	

		// If we got here, we know that we did not timeout
		// So therefore the read from the channel is doen
		// So check if there was an error, if so break
		if (fut.ErrorCode() != 0){
			break;
		}

		// .Get gives us the results of the future
		auto buf = fut.Get();

		// Add what we read to our response
		response.append(buf.Begin(), buf.End());

	}

	return response;


}

void tcp_echo_server(int port, Channel<int> stopchan, use<ITty> out, awaiter await){

	// stopfut will become ready when someone writes to the channel
	auto stopfut = stopchan.Read();

	// Set up our Tcp Server at the specified port
	TcpStream server;
	auto server_addr = Uv::Ip4Addr("0.0.0.0", port);
	server.Bind(server_addr);



	// We use this to tell the client connections they should stop
	auto stopclientchan = make_channel<int>();

	// Start listening
	server.Listen(1, resumable([stopclientchan](use<IUvStream> is, int, awaiter await){	// This is the function that handles the client
		auto stopfut = stopclientchan.Read();

		TcpStream client;
		is.Accept(client);

		// Get the read channel
		auto readchan = client.ReadStartWithChannel();

		// Loop and read the channel, as long as
		while ( !stopfut.Ready()){
		

			// Get the readfuture from the channel
			auto readfut = readchan.Read();

			// Wait for either some data available to read, or else we were told to stop
			await(when_any(readfut,stopfut));

			// Check that the read future is the cause as opposed to being told to stop
			if (readfut.Ready()){
				// Get the buffer
				auto buf = readfut.Get();

				// Generate the http response
				// In this case we are echoing back using http
				std::stringstream strstream;
				strstream <<
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: " << buf.Size() << "\r\n"
					"\r\n";

				strstream.write(buf.Begin(), buf.Size());
				strstream << "\r\n\r\n";

				// Write to the client
				await(client.Write(strstream.str()));
			}

		}
	}));
	// as opposed to await.operator(), await.as_future returns the future
	// This is useful if you don't want to throw an exception on an error in the future
	await.as_future(stopfut);

	// This will close out all the attempted reads on the channel with an error
	stopclientchan.Complete();
}



void uv_main( awaiter await){
	// Tty correspoding to stdin, stdout, stderr
	Tty in{ 0, true };
	Tty out{ 1, false };
	Tty err{ 2, false };

	std::string helpstr = "Quit and press return to quit,\n"
		"Fib to get the latest calculated fibonacci number,\n"
		"Timer <milliseconds> to wait milliseconds,\n"
		"Get <server> <resource> to get an http resource,\n"
		"EchoStart <port> to start an http echo server,\n"
		"EchoList to list ports running echo servers\n"
		"EchoStop <port> to stop the echo server running on port port,\n"
		"Help to display this list of commands,\n"
		"Any other text to have it echoed\n\n";
	// Write out the prompt
	await(out.Write(helpstr));

	// Start the fibonacci writer

	// First make a channels to communicate with the fibonacci calculating function
	auto fibchan = make_channel < std::pair<std::uint16_t, std::uint64_t> >();
	auto stopchan = make_channel<int>();
	// 93 is the largest fibonacci number that will fit in 64bits
	const std::uint16_t fibonacci_n = 93;

	// Uv::Async will run the function on the libuv threadpool
	Uv::Async(std::bind(fibonacci, fibonacci_n, fibchan,stopchan));

	// Start reading and get it as a channel
	auto chan = in.ReadStartWithChannel();
	
	// The value of the nth fibonacci number
	std::uint16_t n = 0;
	std::uint64_t value = 0;

	// Holds all the echoservers
	std::map<int, Channel<int>> running_servers;
	// Loop until we break
	while (true){
		// Read the input channel
		auto buf = await(chan.Read());
		std::string s(buf.Begin(),buf.End());

		// Now handle our commands

		// Quit the program
		if (s.substr(0, 4) == "Quit"){
			stopchan.Write(1);
			break;
		}

		// Fib - get the fibonacci sequence that has been calculated so far
		else if (s.substr(0, 3) == "Fib"){
			std::stringstream s;

			// If n==fibonacci_n, then the calculation is already finished
			if (n < fibonacci_n){
				std::tie(n, value) = await(fibchan.Read());
			}

			s << "Fib(" << n << ") = " << value << "\n";
			await(out.Write(s.str()));
		}

		// Start a timer
		else if (s.substr(0, 5) == "Timer"){

			std::stringstream istr{ s };
			std::string dummy;
			std::uint64_t time{};
			istr >> dummy >> time;
			if (time == 0){
				await(out.Write("Please use a valid time in milliseconds that is greater than 0\n"));
			}else{
				Uv::DefaultExecutor().Add(std::bind(resumable(timer), out, std::chrono::milliseconds{ time }));
			}
		}

		// Get a url
		else if (s.substr(0, 3) == "Get"){
			std::string dummy, server, port, resource;
			std::stringstream istr{ s };
			istr >> dummy >> server >> port >> resource;
			if (server.empty() || port.empty() || resource.empty()){
				await(out.Write("Please enter a valid server, service and resource\n"));
			}
			resumable(get_resource)(server,port, resource).Then(Uv::DefaultExecutor(), [out,server,port,resource](Future<std::string> f)mutable{
				auto ec = f.ErrorCode();
				if (ec < 0){
					std::stringstream s;
					s << "Get encountered error number " << ec << "While getting " << server << " " << port << " " << resource << "\n";
					out.Write(s.str());
				}
				out.Write(f.Get());

			});
		}

		// Start an echo server
		else if (s.substr(0, 9) == "EchoStart"){
			std::stringstream istr{ s };
			std::string dummy;
			int port = 0;
			istr >> dummy >> port;
			if (port == 0 || running_servers.count(port)>0){
				await(out.Write("Please enter a valid port that is unused\n"));
			}
			else{
				auto stopchan = make_channel<int>();
				running_servers[port] = stopchan;
				Uv::DefaultExecutor().Add(
					[stopchan, out, port]()mutable{
						resumable(tcp_echo_server)(port, stopchan, out)
							.Then([out, port](Future<void> f)mutable{
								if (f.ErrorCode()){
									std::stringstream s;
									s << "EchoServer on port " << port << " had error " << f.ErrorCode() << "\n";
									out.Write(s.str()); 
								}
							});
				});
				out.Write("Server started\n");

			}


		}

		// List running echo servers
		else if (s.substr(0, 8) == "EchoList"){
			std::stringstream s;
			s << "Running servers ";
			for (auto& p : running_servers){
				s << p.first << " ";
			}
			s << "\n";
			out.Write(s.str());
		}

		// Stop an echoserver
		else if (s.substr(0, 8) == "EchoStop"){
			std::stringstream istr{ s };
			std::string dummy;
			int port = 0;
			istr >> dummy >> port;
			if (port == 0 || running_servers.count(port) == 0){
				await(out.Write("Please enter a valid port that is in use\n"));
			}
			else{
				running_servers[port].Complete();
				running_servers.erase(port);
				out.Write("Server stopped\n");

			}


		}

		else if (s.substr(0, 4) == "Help"){
			await(out.Write(helpstr));
		}

		// If not a valid command, just echo it back to the screen
		else{
			await(out.Write("You typed " + s + "\n"));
			await(out.Write(helpstr));
		}

	}

}


#include <iostream>
int main(){

	resumable(uv_main)().Then([](Future<void> f){
		// Exit loop at end of scope
		LoopExiter exiter;
		
		//Output any errors
		if (f.ErrorCode()){
			std::cerr << "Error in uv_main " << f.ErrorCode() << "\n";
		}

	});

	// Run the executor loop
	Uv::DefaultExecutor().Loop();

}