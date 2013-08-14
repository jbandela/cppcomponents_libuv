#include "cppcomponents_libuv.hpp"
#include <uv.h>

using namespace cppcomponents;
using namespace cppcomponents_libuv;

namespace clv = cppcomponents_libuv;

inline std::string dummyid(){
	return "4103373e-57db-4928-95e7-17eb88178c2e";
}
//struct IRequest;
//struct IGetAddrinfoRequest;
//struct IShutdownRequest;
//struct IWriteRequest;
//struct IConnectRequest;
//struct IUdpSendRequest;
//struct IUdpRecvRequest;
//struct IFsRequest;
//struct IWorkRequest;


typedef runtime_class<dummyid, object_interfaces<IGetAddrinfoRequest>> GetAddrinfoRequest_t;
typedef runtime_class<dummyid, object_interfaces<IShutdownRequest>> ShutdownRequest_t;
typedef runtime_class<dummyid, object_interfaces<IWriteRequest>> WriteRequest_t;
typedef runtime_class<dummyid, object_interfaces<IConnectRequest>> ConnectRequest_t;
typedef runtime_class<dummyid, object_interfaces<IUdpSendRequest>> UdpSendRequest_t;
typedef runtime_class<dummyid, object_interfaces<IFsRequest>> FsRequest_t;
typedef runtime_class<dummyid, object_interfaces<IWorkRequest>> WorkRequest_t;

template<class Derived, class RC>
struct ImpRequest
	: public implement_runtime_class<Derived, RC>
{
	void* data_;
	uv_req_t& AsReq(){
		return *reinterpret_cast<uv_req_t*>(
			&static_cast<Derived*>(this)->uv_t_);
	}

	int RequestType(){ return AsReq().type; }
	int Cancel(){
		return uv_cancel(&AsReq());

	}
	void* GetData(){ return data_; }
	void SetData(void* v){ data_ = v; };

	void* UvHandle(){
		return &static_cast<Derived*>(this)->uv_t_;
	}

	ImpRequest() : data_(nullptr){}

	typedef ImpRequest imp_t;
};

struct ImpGetAddrinfoRequest
	: public ImpRequest<ImpGetAddrinfoRequest, GetAddrinfoRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;
	using imp_t::UvHandle;


	uv_getaddrinfo_t uv_t_;
	use<GetAddrinfoCallback> cb_;
	use<IGetAddrinfoRequest> self_;
	use<clv::ILoop> loop_;

	use<clv::ILoop> GetLoop(){
		return loop_;
	}


	ImpGetAddrinfoRequest(){
		throw error_not_implemented();
	}
	ImpGetAddrinfoRequest(use<GetAddrinfoCallback> cb, use<clv::ILoop> loop)
		: cb_(cb), self_(this->QueryInterface<IGetAddrinfoRequest>()),
		loop_(loop)

	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_getaddrinfo_t* req, int status, addrinfo* res){
		auto& imp = *static_cast<ImpGetAddrinfoRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.cb_(s, status, res);
	}


};
struct ImpShutdownRequest
	: public ImpRequest<ImpShutdownRequest, ShutdownRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;
	using imp_t::UvHandle;



	uv_shutdown_t uv_t_;
	use<ShutdownCallback> cb_;
	use<IShutdownRequest> self_;
	use<clv::IStream> handle_;

	use<clv::IStream> GetHandle(){
		return handle_;
	}


	ImpShutdownRequest(){
		throw error_not_implemented();
	}
	ImpShutdownRequest(use<ShutdownCallback> cb, use<clv::IStream> h)
		: cb_(cb),
		self_(this->QueryInterface<IShutdownRequest>()),
		handle_(h)
	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_shutdown_t* req, int status){
		auto& imp = *static_cast<ImpShutdownRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.cb_(s, status);
	}


};
struct ImpWriteRequest
	: public ImpRequest<ImpWriteRequest, WriteRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;
	using imp_t::UvHandle;



	uv_write_t uv_t_;
	use<WriteCallback> cb_;
	use<IWriteRequest> self_;
	use<clv::IStream> handle_;
	use<clv::IStream> send_handle_;

	use<clv::IStream> GetHandle(){
		return handle_;
	}
	use<clv::IStream> GetSendHandle(){
		return send_handle_;
	}


	ImpWriteRequest(){
		throw error_not_implemented();
	}
	ImpWriteRequest(use<WriteCallback> cb, use<clv::IStream> h,
		use<clv::IStream> sh = nullptr) : cb_(cb),
		self_(this->QueryInterface<IWriteRequest>()),
		handle_(h), send_handle_(sh)
	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_write_t* req, int status){
		auto& imp = *static_cast<ImpWriteRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.cb_(s, status);
	}


};
struct ImpConnectRequest
	: public ImpRequest<ImpConnectRequest, ConnectRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;
	using imp_t::UvHandle;


	uv_connect_t uv_t_;
	use<ConnectCallback> cb_;
	use<IConnectRequest> self_;
	use<clv::IStream> handle_;

	use<clv::IStream> GetHandle(){
		return handle_;
	}



	ImpConnectRequest(){
		throw error_not_implemented();
	}
	ImpConnectRequest(use<ConnectCallback> cb, use<clv::IStream> h) : cb_(cb),
		self_(this->QueryInterface<IConnectRequest>()),
		handle_(h)
	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_connect_t* req, int status){
		auto& imp = *static_cast<ImpConnectRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.cb_(s, status);
	}


};

struct ImpUdpSendRequest
	: public ImpRequest<ImpUdpSendRequest, UdpSendRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;
	using imp_t::UvHandle;


	uv_udp_send_t uv_t_;
	use<UdpSendCallback> cb_;
	use<IUdpSendRequest> self_;
	use<clv::IStream> handle_;

	use<clv::IStream> GetHandle(){
		return handle_;
	}



	ImpUdpSendRequest(){
		throw error_not_implemented();
	}
	ImpUdpSendRequest(use<UdpSendCallback> cb, use<clv::IStream> h) : cb_(cb),
		self_(this->QueryInterface<IUdpSendRequest>()),
		handle_(h)
	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_udp_send_t* req, int status){
		auto& imp = *static_cast<ImpUdpSendRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.cb_(s, status);
	}


};

struct ImpFsRequest
	: public ImpRequest<ImpFsRequest, FsRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;
	using imp_t::UvHandle;


	uv_fs_t uv_t_;
	use<FsCallback> cb_;
	use<IFsRequest> self_;
	use<clv::ILoop> loop_;

	void Cleanup(){ return uv_fs_req_cleanup(&uv_t_); }

	use<clv::ILoop> GetLoop(){
		return loop_;
	}
	std::ptrdiff_t GetResult(){
		return uv_t_.result;
	}

	void* GetPtr(){
		return uv_t_.ptr;
	}

	cr_string GetPath(){
		return uv_t_.path;
	}

	Stat GetStatBuf(){
		static_assert(sizeof(Stat) == sizeof(uv_stat_t),
			"Stat does not correlate with uv_stat_t");
		Stat ret;
		memcpy(&ret, &uv_t_.statbuf, sizeof(Stat));
		return ret;
	}

	ImpFsRequest(){
		throw error_not_implemented();
	}
	ImpFsRequest(use<FsCallback> cb, use<clv::ILoop> loop,
		use<clv::IStream> sh = nullptr) : cb_(cb),
		self_(this->QueryInterface<IFsRequest>()),
		loop_(loop)
	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_fs_t* req){
		auto& imp = *static_cast<ImpFsRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.cb_(s);
	}


};

struct ImpWorkRequest
	: public ImpRequest<ImpWorkRequest, WorkRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;
	using imp_t::UvHandle;


	uv_work_t uv_t_;
	use<WorkCallback> cb_;
	use<AfterWorkCallback> awcb_;
	use<IWorkRequest> self_;
	use<clv::ILoop> loop_;

	use<clv::ILoop> GetLoop(){
		return loop_;
	}


	ImpWorkRequest(){
		throw error_not_implemented();
	}
	ImpWorkRequest(use<WorkCallback> cb, use<AfterWorkCallback> awcb,
		use<clv::ILoop> loop)
		: cb_(cb), awcb_(awcb),
		self_(this->QueryInterface<IWorkRequest>()),
		loop_(loop)

	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_work_t* req){
		auto& imp = *static_cast<ImpWorkRequest*>(req->data);
		auto s = imp.self_;
		imp.cb_(s);
	}
	static void AfterRequestCb(uv_work_t* req, int status){
		auto& imp = *static_cast<ImpWorkRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.awcb_(s, status);
	}


};


template<class DelegateType>
void* delegate_to_void(use<DelegateType> cb){
	return cb.get_portable_base_addref();
}

template<class DelegateType>
use<DelegateType> delegate_from_void(void* data){
	auto pb = static_cast<portable_base*>(data);
	use<DelegateType> cb(cppcomponents::reinterpret_portable_base<DelegateType::template Interface>(pb), false);
	return cb;
}


typedef runtime_class<dummyid, object_interfaces<IHandle>, factory_interface<NoConstructorFactoryInterface>> Handle_t;

template<class HType>
struct ImpHandleBase{

	uv_handle_t* handle_;
	bool closed_;

	HType* get(){
		return reinterpret_cast<HType*>(handle_);
	}

	ImpHandleBase(HType* h) : handle_{ reinterpret_cast<uv_handle_t*>(h) }, closed_{false}
	{}

	int HandleType(){
		return handle_->type;
	}

	bool IsActive(){
		return uv_is_active(handle_) != 0;
	}

	static void CloseCallbackRaw(uv_handle_t *h){
		auto cb = delegate_from_void<CloseCallback>(h->data);
		h->data = nullptr;
		cb(ImpHandleNonOwning::create(h).QueryInterface<IHandle>());
	}

	void CloseRaw(use<CloseCallback> cb){
		closed_ = true;
		handle_->data = delegate_to_void(cb);
		uv_close(handle_, CloseCallbackRaw);
	}

	void Ref(){
		uv_ref(handle_);
	}

	void Unref(){
		uv_unref(handle_);
	}

	bool HasRef(){
		return uv_has_ref(handle_) != 0;
	}
	bool IsClosing(){
		return uv_is_closing(handle_) != 0;
	}
	void* UvHandle(){
		return handle_;
	}


};

struct ImpHandleNonOwning :
	ImpHandleBase<uv_handle_t>,
	implement_runtime_class<ImpHandleNonOwning, Handle_t>{

		using ImpHandleBase<uv_handle_t>::HandleType;
		using ImpHandleBase<uv_handle_t>::IsActive;
		using ImpHandleBase<uv_handle_t>::CloseRaw;
		using ImpHandleBase<uv_handle_t>::IsClosing;
		using ImpHandleBase<uv_handle_t>::Ref;
		using ImpHandleBase<uv_handle_t>::Unref;
		using ImpHandleBase<uv_handle_t>::HasRef;
		using ImpHandleBase<uv_handle_t>::UvHandle;


		ImpHandleNonOwning(uv_handle_t* h) : ImpHandleBase<uv_handle_t>(h)
		{}



};

struct ImpLoop : implement_runtime_class<ImpLoop, Loop_t>{

	// Whether we own the loop, so we can delete it in destructor
	bool owner_;
	uv_loop_t* loop_;
	ImpLoop() : owner_{ true }, loop_{ uv_loop_new() }
	{
	}

	ImpLoop(uv_loop_t* loop, bool owner = false) : owner_{ false }, loop_{ loop }
	{}

	~ImpLoop(){
		if (owner_){
			uv_loop_delete(loop_);
		}
	}

	static use<ILoop> DefaultLoop(){
		struct uniq{};
		return cross_compiler_interface::detail::safe_static_init < ImpLoop, uniq>::get(uv_default_loop(), false).QueryInterface<ILoop>();
	}

	void Run(){
		throw_if_error(uv_run(loop_, UV_RUN_DEFAULT));
	}
	void RunOnce(){
		throw_if_error(uv_run(loop_, UV_RUN_ONCE));
	}
	void RunNoWait(){
		throw_if_error(uv_run(loop_, UV_RUN_NOWAIT));
	}

	void Stop(){
		uv_stop(loop_);
	}

	void UpdateTime(){
		uv_update_time(loop_);
	}

	std::uint64_t Now(){
		return uv_now(loop_);
	}

	int BackendFd(){
		return uv_backend_fd(loop_);
	}

	int BackendTimeout(){
		return uv_backend_timeout(loop_);
	}

	void* UvHandle(){
		return loop_;
	}

	static void WalkCallbackRaw(uv_handle_t *h, void* arg){
		auto cb = delegate_from_void<WalkCallback>(arg);
		cb(ImpHandleNonOwning::create(h).QueryInterface<IHandle>());
	}

	void WalkRaw(use<WalkCallback> cb){
		uv_walk(loop_, WalkCallbackRaw, delegate_to_void(cb));
	}

	use<IWorkRequest> QueueWorkRaw(use<WorkCallback> wcb, use<AfterWorkCallback> awcb){
		auto wr = ImpWorkRequest::create(wcb, awcb, this->QueryInterface<ILoop>()).QueryInterface<IWorkRequest>();
		uv_queue_work(loop_, reinterpret_cast<uv_work_t*>(wr.UvHandle()),ImpWorkRequest::RequestCb,ImpWorkRequest::AfterRequestCb);
		return wr;
	}



};


uv_buf_t AllocCallbackRaw(uv_handle_t* handle, size_t suggested_size){
	try{
		return uv_buf_init(new char[suggested_size], suggested_size);
	}
	catch (std::exception&){
		return uv_buf_init(nullptr, 0);
	}
}

template<class HType,class Derived,class NonOwning = ImpStreamNonOwning>
struct ImpStreamBase : ImpHandleBase<uv_stream_t>{
	using ImpHandleBase<uv_stream_t>::HandleType;
	using ImpHandleBase<uv_stream_t>::IsActive;
	using ImpHandleBase<uv_stream_t>::CloseRaw;
	using ImpHandleBase<uv_stream_t>::Ref;
	using ImpHandleBase<uv_stream_t>::Unref;
	using ImpHandleBase<uv_stream_t>::HasRef;
	using ImpHandleBase<uv_stream_t>::UvHandle;

	typedef cppcomponents::delegate < void(int) > ConnectionCallbackHelper;
	typedef cppcomponents::delegate < void(std::ptrdiff_t nread, Buffer buf) > ReadCallbackHelper;
	typedef cppcomponents::delegate < void(std::ptrdiff_t nread, Buffer buf, int pending) > Read2CallbackHelper;

	use<IShutdownRequest> Shutdown(cppcomponents::use<ShutdownCallback> cb){
		use<clv::IStream> is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		return ImpShutdownRequest::create(cb, is).QueryInterface<IShutdownRequest>();
	}

	static void ConnectionCallbackRaw(uv_stream_t* server, int status){
		auto cb = delegate_from_void<ConnectionCallbackHelper>(server->data);
		server->data = false;
		cb(status);
	}

	void Listen(int backlog, use<ConnectCallback> cb){

		auto is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		auto cbh = make_delegate<ConnectionCallbackHelper>([is,cb](int status){
			cb(is, status);
		});
		this->handle_->data = delegate_to_void(cbh);
		throw_if_error(uv_listen(this->get(), backlog, ConnectionCallbackRaw));
	}

	use<clv::IStream> Accept(){
		auto is = Derived::create().QueryInterface<clv::IStream>();
		uv_accept(get(), reinterpret_cast<uv_stream_t*>(is.UvHandle()));
		return is;
	}

	static void ReadCallbackRaw(uv_stream_t* stream, ssize_t nread, uv_buf_t buf){
		Buffer b;
		b.base = buf.base;
		b.len = buf.len;

		auto cb = delegate_from_void<ReadCallbackHelper>(stream->data);
		cb(nread, b);

		delete buf.base;
	}

	void ReadStart(use<ReadCallback> cb){
		auto is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		auto cbh = make_delegate<ReadCallbackHelper>([is, cb](std::ptrdiff_t nread, Buffer buf){
			cb(is, nread, buf);
		});
		this->handle_.data = delegate_to_void(cbh);
		uv_read_start(this->get(), AllocCallbackRaw, ReadCallbackRaw);

	}

	void ReadStop(){
		uv_read_stop(this->get());
	}
	static void Read2CallbackRaw(uv_stream_t* stream, ssize_t nread, uv_buf_t buf,int pending){
		Buffer b;
		b.base = buf.base;
		b.len = buf.len;

		auto cb = delegate_from_void<Read2CallbackHelper>(stream->data);
		cb(nread, b,pending);

		delete buf.base;
	}
	void Read2Start(use<Read2Callback> cb){
		auto is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		auto cbh = make_delegate<Read2CallbackHelper>([is, cb](std::ptrdiff_t nread, Buffer buf,int pending){
			cb(is, nread, buf,pending);
		});
		this->handle_.data = delegate_to_void(cbh);
		uv_read_start(this->get(), AllocCallbackRaw, Read2CallbackRaw);

	}

	void Read2Stop(){
		uv_read_stop(this->get());

	}

	use<IWriteRequest> Write(Buffer* bufs, int bufcnt, use<WriteCallback> cb){
		static_assert(sizeof(Buffer) == sizeof(uv_buf_t), "Buffer and uv_buf_t not compatible");
		auto wr = ImpWriteRequest::create(cb, this->QueryInterface<clv::IStream>());
		throw_if_error(uv_write(static_cast<uv_write_t*>(wr.UvHandle()), this->get(), reinterpret_cast<uv_buf_t*>(bufs), bufcnt,
			ImpWriteRequest::RequestCb));

	}	
	use<IWriteRequest> Write2(Buffer* bufs, int bufcnt, use<clv::IStream> is,use<WriteCallback> cb){
		static_assert(sizeof(Buffer) == sizeof(uv_buf_t), "Buffer and uv_buf_t not compatible");
		auto wr = ImpWriteRequest::create(cb, this->QueryInterface<clv::IStream>());
		throw_if_error(uv_write2(static_cast<uv_write_t*>(wr.UvHandle()), this->get(), reinterpret_cast<uv_buf_t*>(bufs), bufcnt,
			static_cast<uv_stream_t*>(is.UvHandle()),ImpWriteRequest::RequestCb));

	}

	bool IsReadable(){
		return uv_is_readable(this->get()) != 0;
	}

	bool IsWritable(){
		return uv_is_writable(this->get()) != 0;
	}

	void SetBlocking(bool blocking){
		uv_stream_set_blocking(this->get(), blocking ? 1 : 0);
	}


};
