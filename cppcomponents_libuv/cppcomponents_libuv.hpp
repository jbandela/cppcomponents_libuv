#include <cppcomponents/cppcomponents.hpp>
#include <atomic>
#include <memory>
#include <uv.h>
template<class T>
struct storage_and_error{
	cppcomponents::error_code error_;

	typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type storage_;
	std::atomic<bool> storage_initialized_;
	std::atomic<bool> finished_;





	storage_and_error() : error_(0), storage_initialized_(false), finished_(false){

	}

	bool finished()const{
		return finished_.load();
	}

	template<class F>
	void do_assign(F& f){
		try{
			void* data = &storage_;
			new(data) T(f());
			storage_initialized_.store(true);

		}
		catch (std::exception& e){
			error_ = cppcomponents::error_mapper::error_code_from_exception(e);
		}
		finished_.store(true);
	}

	void check_get()const {
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


	void set_error(cppcomponents::error_code ec){
		error_ = ec;
		finished_.store(true);
	}

	void set(T t){
		try{
			void* data = &storage_;
			new(data) T(std::move(t));
			storage_initialized_.store(true);

		}
		catch (std::exception& e){
			error_ = cppcomponents::error_mapper::error_code_from_exception(e);
		}
		finished_.store(true);
	}

	~storage_and_error(){
		if (storage_initialized_.load()){
			void* data = &storage_;
			static_cast<const T*>(data)->~T();
		}
	}
};

template<>
struct storage_and_error<void>{
	cppcomponents::error_code error_;

	std::atomic<bool> finished_;





	storage_and_error() : error_(0), finished_(false){

	}

	bool finished()const{
		return finished_.load();
	}

	template<class F>
	void do_assign(F& f){
		try{
			f();

		}
		catch (std::exception& e){
			error_ = cppcomponents::error_mapper::error_code_from_exception(e);
		}
		finished_.store(true);
	}

	void check_get()const{
		if (!finished_.load()){
			throw std::runtime_error("Operation not finished");
		}
		cppcomponents::throw_if_error(error_);

	}
	void set(){
		finished_.store(true);
	}
	void set_error(cppcomponents::error_code ec){
		error_ = ec;
		finished_.store(true);
	}

	void get()const{
		check_get();
	}


	~storage_and_error(){

	}
};

struct mutex{

	mutable uv_mutex_t mu_;
	mutex(){
		uv_mutex_init(&mu_);
	}
	~mutex(){
		uv_mutex_destroy(&mu_);
	}

	void lock(){
		uv_mutex_lock(&mu_);
	}

	int trylock(){
		return uv_mutex_trylock(&mu_);
	}

	void unlock(){
		uv_mutex_unlock(&mu_);
	}

	uv_mutex_t* native()const {
		return &mu_;
	}

};

struct lock{

	mutex& mu_;

	lock(mutex& m) : mu_(m){
		mu_.lock();
	}
	~lock(){
		mu_.unlock();
	}

	mutex& get_mutex(){
		return mu_;
	}

};

struct condition_variable{

	uv_cond_t cond_;

	condition_variable(){
		uv_cond_init(&cond_);
	}

	~condition_variable(){
		uv_cond_destroy(&cond_);
	}

	void wait(lock& lk){
		uv_cond_wait(&cond_, lk.get_mutex().native());
	}
	template<class Pred>
	void wait(lock& lk, Pred pred){
		while (!pred()){
			uv_cond_wait(&cond_, lk.get_mutex().native());

		}
	}

	int timedwait(std::uint64_t n, lock& lk){
		return uv_cond_timedwait(&cond_, lk.get_mutex().native(), n);
	}

	void signal(){
		uv_cond_signal(&cond_);
	}

	void broadcast(){
		uv_cond_broadcast(&cond_);
	}
};

template<class T>
struct value_waiter_helper {

	mutable mutex mu_;
	mutable condition_variable cond_var_;

	storage_and_error<T> storage_;

	T get(){
		lock lk(mu_);
		cond_var_.wait(lk, [this](){return storage_.finished(); });
		return storage_.get();
	}

	void set(T t){
		lock lk(mu_);
		// Only allow 1 store
		if (storage_.finished()) return;
		storage_.set(std::move(t));
		cond_var_.broadcast();
	}

	void set_error(cppcomponents::error_code ec){
		lock lk(mu_);
		// Only allow 1 store
		if (storage_.finished()) return;
		storage_.set_error(ec);
		cond_var_.broadcast();
	}


};

template<>
struct value_waiter_helper<void> {

	mutable mutex mu_;
	mutable condition_variable cond_var_;

	storage_and_error<void> storage_;

	void get()const {
		lock lk(mu_);
		cond_var_.wait(lk, [this](){return storage_.finished(); });
		return storage_.get();
	}

	void set(){
		lock lk(mu_);
		storage_.set();
		cond_var_.broadcast();
	}




};

template<class T>
struct value_waiter{
	typedef value_waiter_helper<T> v_t;
	std::shared_ptr<v_t> p_;

	value_waiter() : p_(std::make_shared<v_t>()){}
	value_waiter(std::shared_ptr<v_t> p) : p_(p){}

	T get()const{
		return p_->get();
	}

	void set(T t){
		p_->set(std::move(t));
	}

};

template<>
struct value_waiter<void>{
	typedef value_waiter_helper<void> v_t;
	std::shared_ptr<v_t> p_;

	value_waiter() : p_(std::make_shared<v_t>()){}
	value_waiter(std::shared_ptr<v_t> p) : p_(p){}

	void get()const{
		return p_->get();
	}

	void set(){
		p_->set();
	}

};