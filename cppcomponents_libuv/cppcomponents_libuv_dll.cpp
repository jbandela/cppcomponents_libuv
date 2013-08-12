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
		: cb_(cb),self_(this->QueryInterface<IGetAddrinfoRequest>()),
		loop_(loop)
		
	{
		uv_t_.data = this;
	}

	static void RequestCb(uv_getaddrinfo_t* req, int status, addrinfo* res){
		auto& imp = *static_cast<ImpGetAddrinfoRequest*>(req->data);
		auto s = imp.self_;
		imp.self_ = nullptr;
		imp.cb_(s, status,res);
	}


}; 
struct ImpShutdownRequest
	: public ImpRequest<ImpShutdownRequest, ShutdownRequest_t>
{

	using imp_t::RequestType;
	using imp_t::Cancel;
	using imp_t::GetData;
	using imp_t::SetData;


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
	ImpWorkRequest(use<WorkCallback> cb,use<AfterWorkCallback> awcb,
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
