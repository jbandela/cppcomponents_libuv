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


uv_loop_t* as_uv_type(use<ILoop> loop){
	return static_cast<uv_loop_t*>(loop.UvHandle());
}
uv_work_t* as_uv_type(use<IWorkRequest> w){
		return static_cast<uv_work_t*>(w.UvHandle());
}
uv_stream_t* as_uv_type(use<clv::IStream> s){
	return static_cast<uv_stream_t*>(s.UvHandle());
}
uv_buf_t* as_uv_type(Buffer* b){
	return reinterpret_cast<uv_buf_t*>(b);
}
uv_connect_t* as_uv_type(use<IConnectRequest> c){
	return static_cast<uv_connect_t*>(c.UvHandle());
}

uv_udp_send_t* as_uv_type(use<IUdpSendRequest> r){
	return static_cast<uv_udp_send_t*>(r.UvHandle());
}
uv_write_t* as_uv_type(use<IWriteRequest> r){
	return static_cast<uv_write_t*>(r.UvHandle());
}


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

	bool closed(){ return closed_; }

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
		uv_queue_work(loop_, as_uv_type(wr),ImpWorkRequest::RequestCb,ImpWorkRequest::AfterRequestCb);
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

template<class HType,class Derived>
struct ImpStreamBase : ImpHandleBase<uv_stream_t>{
	using ImpHandleBase<uv_stream_t>::HandleType;
	using ImpHandleBase<uv_stream_t>::IsActive;
	using ImpHandleBase<uv_stream_t>::CloseRaw;
	using ImpHandleBase<uv_stream_t>::Ref;
	using ImpHandleBase<uv_stream_t>::Unref;
	using ImpHandleBase<uv_stream_t>::HasRef;
	using ImpHandleBase<uv_stream_t>::IsClosing;
	using ImpHandleBase<uv_stream_t>::UvHandle;

	typedef cppcomponents::delegate < void(int) > ConnectionCallbackHelper;
	typedef cppcomponents::delegate < void(std::ptrdiff_t nread, Buffer buf) > ReadCallbackHelper;
	typedef cppcomponents::delegate < void(std::ptrdiff_t nread, Buffer buf, int pending) > Read2CallbackHelper;

	use<clv::IStream> istream_self_;

	use<IShutdownRequest> ShutdownRaw(cppcomponents::use<ShutdownCallback> cb){
		use<clv::IStream> is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		return ImpShutdownRequest::create(cb, is).QueryInterface<IShutdownRequest>();
	}

	static void ConnectionCallbackRaw(uv_stream_t* server, int status){
		auto cb = delegate_from_void<ConnectionCallbackHelper>(server->data);
		server->data = false;
		cb(status);
	}

	void ListenRaw(int backlog, use<ConnectionCallback> cb){

		auto is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		auto cbh = make_delegate<ConnectionCallbackHelper>([is,cb](int status){
			cb(is, status);
		});
		this->handle_->data = delegate_to_void(cbh);
		throw_if_error(uv_listen(this->get(), backlog, ConnectionCallbackRaw));
	}

	void Accept(use<clv::IStream> client){
		throw_if_error(uv_accept(get(), as_uv_type(client)));
	}

	static void ReadCallbackRaw(uv_stream_t* stream, ssize_t nread, uv_buf_t buf){
		Buffer b;
		b.base = buf.base;
		b.len = buf.len;

		auto cb = delegate_from_void<ReadCallbackHelper>(stream->data);
		cb(nread, b);

		delete buf.base;
	}

	void ReadStartRaw(use<ReadCallback> cb){
		auto is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		auto cbh = make_delegate<ReadCallbackHelper>([is, cb](std::ptrdiff_t nread, Buffer buf){
			cb(is, nread, buf);
		});
		this->handle_->data = delegate_to_void(cbh);
		istream_self_ = is;
		try{
		throw_if_error(uv_read_start(this->get(), AllocCallbackRaw, ReadCallbackRaw));
		}
		catch (...){
			istream_self_ = nullptr;
		}

	}

	void ReadStop(){
		istream_self_ = nullptr;
		uv_read_stop(this->get());
	}
	/*
	static void Read2CallbackRaw(uv_stream_t* stream, ssize_t nread, uv_buf_t buf,int pending){
		Buffer b;
		b.base = buf.base;
		b.len = buf.len;

		auto cb = delegate_from_void<Read2CallbackHelper>(stream->data);
		cb(nread, b,pending);

		delete buf.base;
	}
	*/

	// Can't figure out how to use uv_read2_start
	// Plan on implentation after more research
	void Read2StartRaw(use<Read2Callback> cb){
		throw error_not_implemented();

	/*	auto is = static_cast<Derived*>(this)->template QueryInterface<clv::IStream>();
		auto cbh = make_delegate<Read2CallbackHelper>([is, cb](std::ptrdiff_t nread, Buffer buf,int pending){
			cb(is, nread, buf,pending);
		});
		this->handle_->data = delegate_to_void(cbh);
	
		istream_self_ = is;
		try{
			throw_if_error(uv_read2_start(this->get(), AllocCallbackRaw, Read2CallbackRaw));
		}
		catch (...){
			istream_self_ = nullptr;
		}
		*/
	}

	void Read2Stop(){
		throw error_not_implemented();
		/*
		istream_self_ = nullptr;

		uv_read_stop(this->get());
		*/
	}

	use<IWriteRequest> WriteRaw(Buffer* bufs, int bufcnt, use<WriteCallback> cb){
		static_assert(sizeof(Buffer) == sizeof(uv_buf_t), "Buffer and uv_buf_t not compatible");
		auto wr = ImpWriteRequest::create(cb, static_cast<Derived*>(this)->QueryInterface<clv::IStream>()).QueryInterface<IWriteRequest>();
		throw_if_error(uv_write(as_uv_type(wr), this->get(), as_uv_type(bufs), bufcnt,
			ImpWriteRequest::RequestCb));

		return wr;

	}	
	use<IWriteRequest> Write2Raw(Buffer* bufs, int bufcnt, use<clv::IStream> is,use<WriteCallback> cb){
		static_assert(sizeof(Buffer) == sizeof(uv_buf_t), "Buffer and uv_buf_t not compatible");
		auto wr = ImpWriteRequest::create(cb, static_cast<Derived*>(this)->QueryInterface<clv::IStream>()).QueryInterface<IWriteRequest>();;
		throw_if_error(uv_write2(as_uv_type(wr), this->get(), as_uv_type(bufs), bufcnt,
			as_uv_type(is),ImpWriteRequest::RequestCb));
		return wr;
	}

	bool IsReadable(){
		return uv_is_readable(this->get()) != 0;
	}

	bool IsWritable(){
		return uv_is_writable(this->get()) != 0;
	}

	void SetBlocking(bool blocking){
		uv_stream_set_blocking(this->get(), blocking);
	}

	ImpStreamBase(HType* h) : ImpHandleBase(reinterpret_cast<uv_stream_t*>(h)){}



};


struct ImpTcpStream : uv_tcp_t, ImpStreamBase<uv_tcp_t,ImpTcpStream>, implement_runtime_class<ImpTcpStream, TcpStream_t>{

	typedef ImpStreamBase<uv_tcp_t, ImpTcpStream> imp_base_t;

	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;

	ImpTcpStream(use<ILoop> loop) : imp_base_t(this){
		throw_if_error(uv_tcp_init(as_uv_type(loop), this));
	}

	void Open(SocketOsType sock){
		throw_if_error(uv_tcp_open(this, static_cast<uv_os_sock_t>(sock)));
	}

	void NoDelay(bool enable){
		throw_if_error(uv_tcp_nodelay(this, enable));
	}

	void KeepAlive(bool enable, std::uint32_t delay){
		throw_if_error(uv_tcp_keepalive(this, enable, delay));
	}

	void SimultaneousAccepts(bool enable){
		throw_if_error(uv_tcp_simultaneous_accepts(this, enable));
	}

	void Bind(sockaddr_in in){
		throw_if_error(uv_tcp_bind(this, in));
	}

	void Bind6(sockaddr_in6 in6){
		throw_if_error(uv_tcp_bind6(this, in6));
	}

	void GetsocknameRaw(sockaddr* name, int* namelen){
		
		throw_if_error(uv_tcp_getsockname(this, name, namelen));
	}

	void GetpeernameRaw(sockaddr* name, int* namelen){
		throw_if_error(uv_tcp_getpeername(this, name, namelen));
	}

	use<IConnectRequest> ConnectRaw(sockaddr_in address, cppcomponents::use<ConnectCallback> cb){
		auto cr = ImpConnectRequest::create(cb, this->QueryInterface<clv::IStream>()).QueryInterface<IConnectRequest>();
		throw_if_error(uv_tcp_connect(as_uv_type(cr), this, address, ImpConnectRequest::RequestCb));
		return cr;
	}
	use<IConnectRequest> Connect6Raw(sockaddr_in6 address, cppcomponents::use<ConnectCallback> cb){
		auto cr = ImpConnectRequest::create(cb, this->QueryInterface<clv::IStream>()).QueryInterface<IConnectRequest>();
		throw_if_error(uv_tcp_connect6(as_uv_type(cr), this, address, ImpConnectRequest::RequestCb));
		return cr;
	}

	~ImpTcpStream(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}

};

struct ImpUdpStream : uv_udp_t, ImpStreamBase<uv_udp_t, ImpUdpStream>, implement_runtime_class<ImpUdpStream, UdpStream_t>
{
	typedef ImpStreamBase<uv_udp_t, ImpUdpStream> imp_base_t;

	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;

	// This keeps us alive during async requests
	use<IUdpStream> self_;

	ImpUdpStream(use<ILoop> loop) : imp_base_t(this){
		throw_if_error(uv_udp_init(as_uv_type(loop), this));
	}


	void Open(SocketOsType sock){
		throw_if_error(uv_udp_open(this, static_cast<uv_os_sock_t>(sock)));
	}
	void Bind(sockaddr_in in, std::uint32_t flags){
		throw_if_error(uv_udp_bind(this, in, flags));
	}
	void Bind6(sockaddr_in6 in, std::uint32_t flags){
		throw_if_error(uv_udp_bind6(this, in, flags));
	}
	void GetSocknameRaw(sockaddr* name, int* namelen){
		throw_if_error(uv_udp_getsockname(this, name, namelen));
	}
	void SetMembership(cppcomponents::cr_string multicast_addr, cppcomponents::cr_string interface_addr,
		std::uint32_t membership){
			throw_if_error(uv_udp_set_membership(this, multicast_addr.data(), interface_addr.data(), 
				membership==UV_LEAVE_GROUP?UV_LEAVE_GROUP:UV_JOIN_GROUP));

	}
	void SetMulticastLoop(bool on){
		throw_if_error(uv_udp_set_multicast_loop(this, on));
	}
	void SetMulticastTtl(std::int32_t ttl){
		throw_if_error(uv_udp_set_multicast_ttl(this, ttl));
	}
	void SetBroadcast(bool on){
		throw_if_error(uv_udp_set_broadcast(this,on));
	}
	void SetTtl(std::int32_t ttl){
		throw_if_error(uv_udp_set_ttl(this, ttl));
	}
	use<IUdpSendRequest> SendRaw(Buffer* bufs, int buffcnt, sockaddr_in addr,
		cppcomponents::use<UdpSendCallback> cb){
			auto sr = ImpUdpSendRequest::create(cb, this->QueryInterface<clv::IStream>()).QueryInterface<IUdpSendRequest>();
			throw_if_error(uv_udp_send(as_uv_type(sr), this,
				as_uv_type(bufs),buffcnt, addr, ImpUdpSendRequest::RequestCb));
			return sr;
	}
	use<IUdpSendRequest> Send6Raw(Buffer* bufs, int buffcnt, sockaddr_in6 addr,
		cppcomponents::use<UdpSendCallback> cb){
			auto sr = ImpUdpSendRequest::create(cb, this->QueryInterface<clv::IStream>()).QueryInterface<IUdpSendRequest>();
			throw_if_error(uv_udp_send6(as_uv_type(sr), this,
				as_uv_type(bufs), buffcnt, addr, ImpUdpSendRequest::RequestCb));
			return sr;
	}
	static void RecvCallbackRaw(uv_udp_t* handle, ssize_t nread, uv_buf_t buf,
	struct sockaddr* addr, unsigned flags){
		auto& imp = *static_cast <ImpUdpStream*>(handle);

		Buffer b;
		b.base = buf.base;
		b.len = buf.len;


		auto cb = delegate_from_void<UdpRecvCallback>(handle->data);
		cb(imp.self_, nread, b,addr,flags);

		delete buf.base;
	}


	void RecvStartRaw(cppcomponents::use<UdpRecvCallback> cb){
		this->data = delegate_to_void(cb);
		self_ = this->QueryInterface<IUdpStream>();
		try{
			throw_if_error(uv_udp_recv_start(this, AllocCallbackRaw, RecvCallbackRaw));

		}
		catch (...){
			self_ = nullptr;
			throw;
		}
	}
	void RecvStop(){
		self_ = nullptr;
		throw_if_error(uv_udp_recv_stop(this)); 
	}
	~ImpUdpStream(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}

};


struct ImpTtyStream : uv_tty_t, ImpStreamBase<uv_tty_t, ImpTtyStream>, implement_runtime_class<ImpTtyStream, Tty_t>
{
	typedef ImpStreamBase<uv_tty_t, ImpTtyStream> imp_base_t;
	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;


	ImpTtyStream(use<ILoop> loop, FileOsType fd, bool readable)
		: imp_base_t(this)
	{
		throw_if_error(uv_tty_init(as_uv_type(loop), this, fd, readable));
	}

	void SetMode(int mode){
		throw_if_error(uv_tty_set_mode(this, mode));
	}

	std::pair<int, int> GetWinsize(){
		std::pair<int, int> ret;
		throw_if_error(uv_tty_get_winsize(this, &ret.first, &ret.second));
		return ret;
	}

	static void ResetMode(){
		uv_tty_reset_mode();
	}

	~ImpTtyStream(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}


};


struct ImpPipe : uv_pipe_t, ImpStreamBase<uv_pipe_t, ImpPipe>, implement_runtime_class<ImpPipe, Pipe_t>
{
	typedef ImpStreamBase<uv_pipe_t, ImpPipe> imp_base_t;
	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;


	ImpPipe(use<ILoop> loop, bool ipc) : imp_base_t(this)
	{
		throw_if_error(uv_pipe_init(as_uv_type(loop), this, ipc));
	}

	void Open(FileOsType file){
		throw_if_error(uv_pipe_open(this, file));
	}

	void Bind(cr_string name){
		throw_if_error(uv_pipe_bind(this, name.data()));
	}

	use<IConnectRequest> ConnectRaw(cr_string name, use<ConnectCallback> cb){
		auto cr = ImpConnectRequest::create(cb, this->QueryInterface<clv::IStream>()).QueryInterface<IConnectRequest>();
		uv_pipe_connect(as_uv_type(cr), this, name.data(), ImpConnectRequest::RequestCb);
		return cr;
	}

	void PendingInstances(int count){
		uv_pipe_pending_instances(this, count);
	}

	~ImpPipe(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}
};

struct ImpPoll : uv_poll_t, ImpStreamBase<uv_poll_t, ImpPoll>, implement_runtime_class<ImpPoll, Poll_t>
{
	typedef ImpStreamBase<uv_poll_t, ImpPoll> imp_base_t;
	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;

	use<IPoll> poll_self_;

	ImpPoll(use<ILoop> loop, FileOsType file) : imp_base_t(this){
		throw_if_error(uv_poll_init(as_uv_type(loop), this, file));
	}
	ImpPoll(use<ILoop> loop, SocketOsType sock) : imp_base_t(this){
		throw_if_error(uv_poll_init_socket(as_uv_type(loop), this, static_cast<uv_os_sock_t>(sock)));
	}

	static void PollCallbackRaw(uv_poll_t* handle, int status, int events){
		auto& imp = *static_cast<ImpPoll*>(handle);
		auto cb = delegate_from_void<PollCallback>(handle->data);
		cb(imp.poll_self_, status, events);

	}

	void StartRaw(int events, use<PollCallback> cb){
		this->data = delegate_to_void(cb);
		poll_self_ = this->QueryInterface<IPoll>();
		try{
			throw_if_error(uv_poll_start(this, events,PollCallbackRaw));

		}
		catch (...){
			poll_self_ = nullptr;
			throw;
		}
	}

	void Stop(){
		poll_self_ = nullptr;
		throw_if_error(uv_poll_stop(this));
	}
	~ImpPoll(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}
	
};

struct ImpPrepare : uv_prepare_t, ImpStreamBase<uv_prepare_t, ImpPrepare>, implement_runtime_class<ImpPrepare, Prepare_t>
{
	typedef ImpStreamBase<uv_prepare_t, ImpPrepare> imp_base_t;
	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;

	use<IPrepare> prepare_self_;

	ImpPrepare(use<ILoop> loop) : imp_base_t(this){
		throw_if_error(uv_prepare_init(as_uv_type(loop), this));
	}


	static void PrepareCallbackRaw(uv_prepare_t* handle, int status){
		auto& imp = *static_cast<ImpPrepare*>(handle);
		auto cb = delegate_from_void<PrepareCallback>(handle->data);
		cb(imp.prepare_self_, status);

	}

	void StartRaw(use<PrepareCallback> cb){
		this->data = delegate_to_void(cb);
		prepare_self_ = this->QueryInterface<IPrepare>();
		try{
			throw_if_error(uv_prepare_start(this,  PrepareCallbackRaw));

		}
		catch (...){
			prepare_self_ = nullptr;
			throw;
		}
	}

	void Stop(){
		prepare_self_ = nullptr;
		throw_if_error(uv_prepare_stop(this));
	}

	~ImpPrepare(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}

};

struct ImpCheck : uv_check_t, ImpStreamBase<uv_check_t, ImpCheck>, implement_runtime_class<ImpCheck, Check_t>
{
	typedef ImpStreamBase<uv_check_t, ImpCheck> imp_base_t;
	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;

	use<ICheck> check_self_;

	ImpCheck(use<ILoop> loop) : imp_base_t(this){
		throw_if_error(uv_check_init(as_uv_type(loop), this));
	}


	static void CheckCallbackRaw(uv_check_t* handle, int status){
		auto& imp = *static_cast<ImpCheck*>(handle);
		auto cb = delegate_from_void<CheckCallback>(handle->data);
		cb(imp.check_self_, status);

	}

	void StartRaw(use<CheckCallback> cb){
		this->data = delegate_to_void(cb);
		check_self_ = this->QueryInterface<ICheck>();
		try{
			throw_if_error(uv_check_start(this, CheckCallbackRaw));

		}
		catch (...){
			check_self_ = nullptr;
			throw;
		}
	}

	void Stop(){
		check_self_ = nullptr;
		throw_if_error(uv_check_stop(this));
	}
	~ImpCheck(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}

};


struct ImpIdle : uv_idle_t, ImpStreamBase<uv_idle_t, ImpIdle>, implement_runtime_class<ImpIdle, Idle_t>
{
	typedef ImpStreamBase<uv_idle_t, ImpIdle> imp_base_t;
	using imp_base_t::HandleType;
	using imp_base_t::IsActive;
	using imp_base_t::CloseRaw;
	using imp_base_t::Ref;
	using imp_base_t::Unref;
	using imp_base_t::HasRef;
	using imp_base_t::IsClosing;
	using imp_base_t::UvHandle;

	using imp_base_t::ShutdownRaw;
	using imp_base_t::ListenRaw;
	using imp_base_t::Accept;

	using imp_base_t::ReadStartRaw;
	using imp_base_t::ReadStop;
	using imp_base_t::Read2StartRaw;
	using imp_base_t::WriteRaw;
	using imp_base_t::Write2Raw;
	using imp_base_t::IsReadable;
	using imp_base_t::IsWritable;
	using imp_base_t::SetBlocking;

	use<IIdle> idle_self_;

	ImpIdle(use<ILoop> loop) : imp_base_t(this){
		throw_if_error(uv_idle_init(as_uv_type(loop), this));
	}


	static void IdleCallbackRaw(uv_idle_t* handle, int status){
		auto& imp = *static_cast<ImpIdle*>(handle);
		auto cb = delegate_from_void<IdleCallback>(handle->data);
		cb(imp.idle_self_, status);

	}

	void StartRaw(use<IdleCallback> cb){
		this->data = delegate_to_void(cb);
		idle_self_ = this->QueryInterface<IIdle>();
		try{
			throw_if_error(uv_idle_start(this, IdleCallbackRaw));

		}
		catch (...){
			idle_self_ = nullptr;
			throw;
		}
	}

	void Stop(){
		idle_self_ = nullptr;
		throw_if_error(uv_idle_stop(this));
	}
	~ImpIdle(){
		if (!this->closed()){
			uv_close(this->handle_, nullptr);
		}
	}

};

