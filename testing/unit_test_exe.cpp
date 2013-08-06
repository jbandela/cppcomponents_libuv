#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Psapi.lib")
#include <stdio.h>
#include <stdlib.h>
#include <atomic>

#include <uv.h>
#include <thread>
#include <chrono>
#include <type_traits>
#include <cppcomponents/cppcomponents.hpp>
#include <memory>
#include <sstream>
#include <string>

#define FIB_UNTIL 25
//uv_loop_t *loop;


void print_tid(const std::string& str  = "On Thread "){
	std::stringstream s;
	s << str << " " << std::this_thread::get_id() << "\n";

	fputs(s.str().c_str(),stderr);
}

template<class T>
struct work:std::enable_shared_from_this<work<T>>{

	uv_work_t w_;
	std::atomic_flag then_called_;
	std::atomic<bool> finished_;
	std::atomic_flag has_then_;
	std::function < void()> then_;
	std::function < T()> worker;
	std::atomic<bool> storage_initialized_;
	cppcomponents::error_code error_;

	typename std::aligned_storage<sizeof(T),std::alignment_of<T>::value>::type storage_;

	std::shared_ptr<work> shared_self_;

	bool then_added_;


	template<class F>
	work(F f)
		: w_{}, then_called_{ ATOMIC_FLAG_INIT },
		finished_{ false }, has_then_{ ATOMIC_FLAG_INIT }, worker(f), error_(0), storage_initialized_{false}, then_added_(false)
	{
		w_.data = this;
	}

	void start(){
		shared_self_ = this->shared_from_this();
		uv_queue_work(uv_default_loop(), &w_, do_work, after_work_cb);
	}
	void start_on_default_loop(){
		shared_self_ = this->shared_from_this();
		uv_queue_work(uv_default_loop(), &w_, do_no_work, work_on_default_loop);
	}


	void check_get(){
		if (!finished_.load()){
			throw std::runtime_error("Operation not finished");
		}
		cppcomponents::throw_if_error(error_);
		if (!storage_initialized_.load()){
			throw std::runtime_error("Storage not initialized");
		}
	}

	T& get(){
		check_get(); 

		void* data = &storage_;
		auto& ret = *static_cast<T*>(data);
		return ret;
	}
	const T& get()const{
		check_get();
		const void* data = &storage_;
		return *static_cast<const T*>(data);
	}

	template<class TF>
	auto then(bool use_default_loop,TF f)->std::shared_ptr<work<decltype(f(shared_self_))>>{
		if (then_added_){
			throw std::runtime_error("Then called more than once");
		}
		then_added_ = true;
		typedef decltype(f(shared_self_)) R;
		auto self_ = shared_self_;
		auto pw = std::make_shared<work<R>>([self_, f](){
			return f(self_);

		});
		then_ = [pw, use_default_loop](){
			if (use_default_loop){
				pw->start_on_default_loop();
			}
			else{
				pw->start();
			}
		};
		has_then_.test_and_set();
		if (finished_.load()){
			if (then_called_.test_and_set() == false){
				then_();
				then_ = [](){};
			}
		}

		return pw;
	}

	template<class TF>
	auto then(TF f)->decltype(then(false, f)){
		return then(false, f);
	}
	static void do_work(uv_work_t* wt){
		auto& w = *static_cast<work*>(wt->data);
		void* data = &w.storage_;
		try{
			new(data) T(w.worker());
			w.storage_initialized_.store(true);

		}
		catch (std::exception& e){
			w.error_ = cppcomponents::error_mapper::error_code_from_exception(e);
		}
		w.finished_.store(true);
		if (w.has_then_.test_and_set()){
			if (w.then_called_.test_and_set()==false){
				w.then_();		
				w.then_ = [](){};
			}

		}
	}

	

	static void after_work_cb(uv_work_t* req, int status){
		auto& w = *static_cast<work*>(req->data);
		w.shared_self_ = nullptr;
		print_tid("In after work");
	}

	// For then on main loop
	static void do_no_work(uv_work_t*){}

	static void work_on_default_loop(uv_work_t* req, int status){
		do_work(req);
		after_work_cb(req, status);
	}


	~work(){
		if (storage_initialized_.load()){
			void* data = &storage_;
			static_cast<const T*>(data)->~T();
		}
	}
};


long fib_(long t) {
	if (t == 0 || t == 1)
		return 1;
	else
		return fib_(t - 1) + fib_(t - 2);
}

long fib(int n) {
	if (rand() % 2)
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	else
		std::this_thread::sleep_for(std::chrono::milliseconds(3));
	long f = fib_(n);
	return f;
}

void after_fib(uv_work_t *req, int status) {
	fprintf(stderr, "Done calculating %dth fibonacci\n", *(int *) req->data);
}

int main() {
	//loop = uv_default_loop();
	print_tid("Main thread");
	auto w = std::make_shared<work<long>>([](){return fib(12); });
	w->start();
	w->then(true,[](std::shared_ptr < work < long >> w){
		auto f = w->get();
		fprintf(stderr, "%dth fibonacci is %lu\n", 12,f);
		print_tid("In then");
		return 0;
	});


	return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}