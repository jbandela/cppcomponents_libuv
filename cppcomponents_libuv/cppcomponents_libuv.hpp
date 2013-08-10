#include <cppcomponents/cppcomponents.hpp>

// Define our buffer structure
#if defined(__SVR4) && !defined(__unix__)
# define __unix__
#endif

#if defined(__unix__) || defined(__POSIX__) || \
	defined(__APPLE__) || defined(_AIX)

// from uv-unix.h
/* Note: May be cast to struct iovec. See writev(2). */
/*typedef struct {
	char* base;
	size_t len;
} uv_buf_t;
*/

#include <netinet/in.h>

namespace cppcomponents_libuv{
	struct Buffer{
		char* base;
		size_t len;
	};

	typedef int FileOsType;
	typedef int SocketOsType;
}
#else
#include <winsock.h>
/**
* It should be possible to cast uv_buf_t[] to WSABUF[]
* see http://msdn.microsoft.com/en-us/library/ms741542(v=vs.85).aspx
*/
/*typedef struct uv_buf_t {
	ULONG len;
	char* base;
} uv_buf_t;*/
namespace cppcomponents_libuv{
	struct Buffer {
		unsigned long len;
		char* base;
	};

	typedef std::uint64_t SocketOsType;
	typedef int FileOsType;


	// Copied from uv.h
	typedef struct {
		long tv_sec;
		long tv_nsec;
	} TimeSpec;


	typedef struct {
		uint64_t st_dev;
		uint64_t st_mode;
		uint64_t st_nlink;
		uint64_t st_uid;
		uint64_t st_gid;
		uint64_t st_rdev;
		uint64_t st_ino;
		uint64_t st_size;
		uint64_t st_blksize;
		uint64_t st_blocks;
		uint64_t st_flags;
		uint64_t st_gen;
		TimeSpec st_atim;
		TimeSpec st_mtim;
		TimeSpec st_ctim;
		TimeSpec st_birthtim;
	} Stat;

}
#endif
struct addrinfo;

 namespace cross_compiler_interface{

	 template<>
	 struct cross_conversion<cppcomponents_libuv::Buffer>
		 :public trivial_conversion<cppcomponents_libuv::Buffer>{};
	 template<>
	 struct cross_conversion<cppcomponents_libuv::Buffer*>
		 :public trivial_conversion<cppcomponents_libuv::Buffer*>{};
	 template<>
	 struct cross_conversion<const cppcomponents_libuv::Buffer*>
		 :public trivial_conversion<const cppcomponents_libuv::Buffer*>{};

	 template<>
	 struct cross_conversion<sockaddr>
		 :public trivial_conversion<sockaddr>{};
	 template<>
	 struct cross_conversion<sockaddr*>
		 :public trivial_conversion<sockaddr*>{};
	 template<>
	 struct cross_conversion<const sockaddr*>
		 :public trivial_conversion<const sockaddr*>{};	
	 
	 template<>
	 struct cross_conversion<sockaddr_in6>
		 :public trivial_conversion<sockaddr_in6>{};
	 template<>
	 struct cross_conversion<sockaddr_in6*>
		 :public trivial_conversion<sockaddr_in6*>{};
	 template<>
	 struct cross_conversion<const sockaddr_in6*>
		 :public trivial_conversion<const sockaddr_in6*>{};

	 template<>
	 struct cross_conversion<sockaddr_in>
		 :public trivial_conversion<sockaddr_in>{};
	 template<>
	 struct cross_conversion<sockaddr_in*>
		 :public trivial_conversion<sockaddr_in*>{};
	 template<>
	 struct cross_conversion<const sockaddr_in*>
		 :public trivial_conversion<const sockaddr_in*>{};

	 template<>
	 struct cross_conversion<addrinfo*>
		 :public trivial_conversion<addrinfo*>{};


	 template<>
	 struct cross_conversion<cppcomponents_libuv::TimeSpec>
		 :public trivial_conversion<cppcomponents_libuv::TimeSpec>{};
	 template<>
	 struct cross_conversion<cppcomponents_libuv::TimeSpec*>
		 :public trivial_conversion<cppcomponents_libuv::TimeSpec*>{};
	 template<>
	 struct cross_conversion<cppcomponents_libuv::TimeSpec>
		 :public trivial_conversion<cppcomponents_libuv::TimeSpec>{};
	 template<>
	 struct cross_conversion<cppcomponents_libuv::Stat*>
		 :public trivial_conversion<cppcomponents_libuv::Stat*>{};


 }

namespace cppcomponents_libuv{


	using cppcomponents::use;
	using cppcomponents::define_interface;
	

	// All the Interfaces declared

	// Requests

	struct IRequest;
	struct IGetAddrinfoRequest;
	struct IShutdownRequest;
	struct IWriteRequest;
	struct IConnectRequest;
	struct IUdpSendRequest;
	struct IFsRequest;
	struct IWorkRequest;


	// Handles
	struct ILoop ;
	struct IHandle ;
	struct IStream ;
	struct ITcp ;
	struct IUdp ;
	struct IPipe ;
	struct ITty ;
	struct IPoll ;
	struct ITimer ;
	struct IPrepare ;
	struct ICheck ;
	struct IIdle ;
	struct IAsync ;
	struct IProcess ;
	struct IFsEvents ;
	struct IFsPoll ;
	struct ISignal ;


	// Callbacks
	typedef cppcomponents::delegate < Buffer(use<IHandle> handle, std::ptrdiff_t suggested_size),
		cppcomponents::uuid<0x3298fd84, 0xe097, 0x4fc5, 0x9b28, 0x60e1f5e5d0c8	>
	> AllocCallback;

	typedef cppcomponents::delegate < void(use<IStream> stream, std::ptrdiff_t, Buffer),
		cppcomponents::uuid<0x38df861d, 0x421b, 0x43d1, 0xb081, 0x7d2ad030c44c		>
	> ReadCallback;

	typedef cppcomponents::delegate < void(use<IStream> pipe, std::ptrdiff_t nread, Buffer buf,
		int pending),
		cppcomponents::uuid<0xd4efdca8, 0xc398, 0x465a, 0x8812, 0x653250b6cdb4		>
	> Read2Callback;

	typedef cppcomponents::delegate < void(use<IWriteRequest>, int status), 
		cppcomponents::uuid<0x1c150cb2, 0xde88, 0x46cf, 0xbfad, 0xeb6ede7611ed> 
	> WriteCallback;

	typedef cppcomponents::delegate < void(use<IConnectRequest>, int),
		cppcomponents::uuid<0x51a43254, 0x1c9b, 0x462e, 0xacc3, 0xecb908249e95> 
	> ConnectCallback;

	typedef cppcomponents::delegate < void(use<IShutdownRequest>, int),
		cppcomponents::uuid<0x121276b6, 0x4d00, 0x4db3, 0xb0e2, 0xbf761a031162>
	> ShutdownCallback;

	typedef cppcomponents::delegate < void(use<IStream>, int),
		cppcomponents::uuid<0x2965d620, 0xac78, 0x4f07, 0xb10b, 0x748401d69656>
	> ConnectionCallback;

	typedef cppcomponents::delegate < void(use<IHandle> handle),
		cppcomponents::uuid<0x5287f549, 0x9b31, 0x4707, 0x93d0, 0xf0ac3fec8b45>
	> CloseCallback;

	typedef cppcomponents::delegate < void(use<IPoll>,int status, int events),
		cppcomponents::uuid<0x1679860e, 0x3e67, 0x4b1b, 0x8791, 0x18080f1df647> 
	> PollCallback;

	typedef cppcomponents::delegate < void(use<ITimer>, int status),
		cppcomponents::uuid<0xccecc946, 0x1bcd, 0x48c8, 0x818a, 0xbd90445eee11	> 
	> TimerCallback;

	typedef cppcomponents::delegate < void(use<IAsync>, int status),
		cppcomponents::uuid<0x5259a184, 0x0903, 0x4694, 0xbb4c, 0xa83f241b10da>
	> AsyncCallback;

	typedef cppcomponents::delegate < void(use<IPrepare>, int status),
		cppcomponents::uuid<0x66854dad, 0x0408, 0x4c4e, 0xafcd, 0x5a2fe7366914>
	> PrepareCallback;

	typedef cppcomponents::delegate < void(use<ICheck>, int status),
		cppcomponents::uuid<0x5817e0e6, 0x31c8, 0x40ab, 0xbce5, 0x6913db56a285>
	> CheckCallback;

	typedef cppcomponents::delegate < void(use<IIdle>, int status),
		cppcomponents::uuid<0x05727499, 0xcc1d, 0x4170, 0xa88c, 0xe31f8d014778>
	> IdleCallback;

	typedef cppcomponents::delegate < void(use<IProcess>, int status, int term_signal),
		cppcomponents::uuid<0xa585d683, 0x22eb, 0x453d, 0xbcc4, 0x79d4c328afb1>
	> ExitCallback;

	typedef cppcomponents::delegate < void(use<IHandle>, void* arg),
		cppcomponents::uuid<0x6b443ec5, 0x32ca, 0x4d3d, 0x99e0, 0xb2b1a33c3111>
	> WalkCallback;

	typedef cppcomponents::delegate < void(use<IFsRequest>),
		cppcomponents::uuid<0x51cc28e9, 0xb06d, 0x49e9, 0x85c4, 0x008a37780ea9>
	> FsCallback;

	typedef cppcomponents::delegate < void(use<IWorkRequest>),
		cppcomponents::uuid<0x00387da7, 0x2931, 0x4f8f, 0xaa6e, 0x66afb7c0c4a0>
	> WorkCallback;

	typedef cppcomponents::delegate < void(use<IWorkRequest>, int status),
		cppcomponents::uuid<0xf2cd0ccb, 0x3307, 0x438c, 0x8e7b, 0x4b52f563129a>
	> AfterWorkCallback;

	typedef cppcomponents::delegate < void (use<IGetAddrinfoRequest>, int status, addrinfo* res),
		cppcomponents::uuid < 0x3249125d, 0x8a0b, 0x488c, 0xbf11, 0x9968f4e8a85d >
	> GetaddrinfoCallback;

	typedef cppcomponents::delegate < void(use<IUdpSendRequest>, int status),
		cppcomponents::uuid<0x719a0a9f, 0x6834, 0x47c5, 0xb659, 0x1fff4784bee7>
	> UdpSendCallback;











	struct IRequest
		: public cppcomponents::define_interface < cppcomponents::uuid < 0xd6d00c3e , 0x5a30 , 0x48c9 , 0x9f48 , 0x9f535bdc4af6	>>
	{
		int RequestType();
		int Cancel();
		void* GetData();
		void* SetData(void*);

		CPPCOMPONENTS_CONSTRUCT(IRequest, RequestType, Cancel,GetData,SetData);
	};


	struct IShutdownRequest
		: public cppcomponents::define_interface < cppcomponents::uuid<0x54fdda53, 0xa752, 0x434b, 0x8103, 0xbf35351a3b22>,
		IRequest>
	{
		cppcomponents::use<IStream> GetHandle();

		CPPCOMPONENTS_CONSTRUCT(IShutdownRequest, GetHandle);
	};

	struct IWriteRequest
		: public cppcomponents::define_interface < cppcomponents::uuid<0xe3e326d2, 0x49ea, 0x4193, 0x9508, 0x108a95c8a29e> ,
		IRequest>
	{
		cppcomponents::use<IStream> GetHandle();
		cppcomponents::use<IStream> GetSendHandle();

		CPPCOMPONENTS_CONSTRUCT(IShutDownRequest, GetHandle,GetSendHandle);
	};

	struct IConnectRequest
		: public cppcomponents::define_interface < cppcomponents::uuid<0x29c0c28e, 0x5a17, 0x433a, 0xa06a, 0x1af0a2e98558>,
		IRequest>
	{
		cppcomponents::use<IStream> GetHandle();

		CPPCOMPONENTS_CONSTRUCT(IConnectRequest, GetHandle);
	};
	struct IUdpSendRequest
		: public cppcomponents::define_interface < cppcomponents::uuid<0xc838dab9, 0x8815, 0x4592, 0x84f4, 0x3574ea72db0d>,
		IRequest>
	{
		cppcomponents::use<IStream> GetHandle();

		CPPCOMPONENTS_CONSTRUCT(IUdpSendRequest, GetHandle);
	};


	struct IGetAddrinfoRequest 
		: public cppcomponents::define_interface < cppcomponents::uuid<0x9c3fbcb4, 0xa35e, 0x4486, 0x9377, 0xedb1262e5f6e>,
		IRequest>
	{
		cppcomponents::use<ILoop> GetLoop();

		CPPCOMPONENTS_CONSTRUCT(IGetAddrinfoRequest, GetLoop);
	};

	struct IWorkRequest
		: public cppcomponents::define_interface < cppcomponents::uuid<0xb2466973, 0xc746, 0x4730, 0x9c7b, 0x944b60607e11>,
		IRequest>
	{
		cppcomponents::use<ILoop> GetLoop();

		CPPCOMPONENTS_CONSTRUCT(IWorkRequest, GetLoop);
	};

	struct IFsRequest
		: public cppcomponents::define_interface < cppcomponents::uuid<0x50d9617d, 0xf3e2, 0x4c2a, 0x909f, 0x65c810fa3340>,
		IRequest>
	{
		void Cleanup();
		cppcomponents::use<ILoop> GetLoop();
		std::ptrdiff_t GetResult();
		void* GetPtr();
		cppcomponents::cr_string GetPath();
		Stat GetStatBuf();


		CPPCOMPONENTS_CONSTRUCT(IFsRequest,Cleanup, GetLoop,GetResult,GetPtr,GetPath,GetStatBuf);
	};

	struct IUdpSendRequest
		: public cppcomponents::define_interface < cppcomponents::uuid<0x002d6f99, 0xc330, 0x43f2, 0xa7d0, 0x9e9a9c85f50d>,
		IRequest>
	{
		cppcomponents::use<IStream> GetHandle();

		CPPCOMPONENTS_CONSTRUCT(IUdpSendRequest, GetHandle);
	};



	struct IHandle
		: cppcomponents::define_interface<cppcomponents::uuid<
		0xdeb1580e , 0x08e2 , 0x4f67 , 0xa03c , 0x90d155bb04be
		>>
	{
		int HandleType();
		bool IsActive();
		void Close(cppcomponents::use<CloseCallback>);
		void* UvHandle();

		CPPCOMPONENTS_CONSTRUCT(IHandle, IsActive,HandleType,Close,UvHandle);
	};
	typedef cppcomponents::delegate < void(cppcomponents::use<IHandle>),
		cppcomponents::uuid < 0x6e965f49, 0xc378, 0x4549, 0xa332, 0x9e4d46fd4041
		>> WalkCallback;





	struct ILoop
		: cppcomponents::define_interface < cppcomponents::uuid < 0xfc3c5e3c , 0x10af , 0x47a3 , 0x89fc , 0xdaea39cb9d58>>
	{
		int RunDefault();
		int RunOnce();
		int RunNoWait();

		void Stop();
		void Ref();
		void UnRef();
		int HasRef();

		void UpdateTime();
		std::uint64_t Now();
		int BackendFd();
		int BackendTimeout();

		void* UvHandle();

		void Walk(cppcomponents::use<WalkCallback>,void* arg);

		use<IWorkRequest> QueueWork(cppcomponents::use<WorkCallback>, cppcomponents::use<AfterWorkCallback>);


		CPPCOMPONENTS_CONSTRUCT(ILoop, RunDefault, RunOnce, RunNoWait,
			Stop,Ref,UnRef,HasRef,UpdateTime,Now,BackendFd, BackendTimeout
			,UvHandle,Walk);
	};

	struct ILoopStatics
		: cppcomponents::define_interface < cppcomponents::uuid < 0xd84fa990, 0xccb8, 0x4480, 0x8a3c, 0xb494ef8d79a0>>
	{
		cppcomponents::use<ILoop> DefaultLoop();
		CPPCOMPONENTS_CONSTRUCT(ILoopStatics, DefaultLoop);
	};

	inline std::string LoopId(){ return "cppcomponents_libuv_dll!Loop"; }
	typedef cppcomponents::runtime_class<LoopId, cppcomponents::object_interfaces<ILoop>, cppcomponents::static_interfaces<ILoopStatics>> Loop_t;
	typedef cppcomponents::use_runtime_class<Loop_t> Loop;

	


	
	
	struct IStream
		: public cppcomponents::define_interface < cppcomponents::uuid < 
		0x9b7c72d8 , 0xb955 , 0x4163 , 0x9e1f , 0x0c905b60c58f
		>,IHandle>
	{
		use<IShutdownRequest> Shutdown(cppcomponents::use<ShutdownCallback>);
		void Listen(int backlog, cppcomponents::use<ConnectionCallback>);
		use<IStream> Accept();

		void ReadStart(cppcomponents::use<AllocCallback>,cppcomponents::use<ReadCallback>);
		void ReadStop();
		void Read2Start(cppcomponents::use<AllocCallback>, cppcomponents::use<Read2Callback>);
		void Write(Buffer* bufs, int bufcnt, cppcomponents::use<WriteCallback>);
		void Write2(Buffer* bufs, int bufcnt, cppcomponents::use <IStream>, cppcomponents::use<WriteCallback>);
		bool IsReadable();
		bool IsWritable();
		void SetBlocking(bool blocking);
		bool IsClosing();




		CPPCOMPONENTS_CONSTRUCT(IStream, Shutdown,Listen,Accept,ReadStart,ReadStop,Read2Start,Write,
			Write2,IsReadable,IsWritable,SetBlocking,IsClosing);

	};


	struct ITcpStream
		: public cppcomponents::define_interface < cppcomponents::uuid <
		0x7e57d378 , 0xa97c , 0x463d , 0xb153 , 0xd7c414ab4c59
		>,IStream>
	{
		void Open(SocketOsType sock);
		void NoDelay(bool enable);
		void KeepAlive(bool enable, std::uint32_t delay);
		void SimultaneousAccepts(bool enable);
		void Bind(sockaddr_in);
		void Bind6(sockaddr_in6);
		void GetSockName(sockaddr* name, int* namelen);
		void GetPeerName(sockaddr* name, int namelen);
		use<IConnectRequest> Connect(sockaddr_in address, cppcomponents::use<ConnectCallback>);
		use<IConnectRequest> Connect6(sockaddr_in6 address, cppcomponents::use<ConnectCallback>);

		CPPCOMPONENTS_CONSTRUCT(ITcpStream, Open,NoDelay,KeepAlive,SimultaneousAccepts,
			Bind,Bind6,GetSockName,GetPeerName,Connect,Connect6);
	};

	struct ITcpStreamFactory
		: public cppcomponents::define_interface <
		cppcomponents::uuid
		<0x83be8a93 , 0x5883 , 0x4961 , 0xb885 , 0xab20a4b93919>>
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> TcpInit(cppcomponents::use<ILoop>);

		CPPCOMPONENTS_CONSTRUCT(ITcpStreamFactory, TcpInit);
	};

	struct IUdpStream
		: public cppcomponents::define_interface < cppcomponents::uuid <
		0x2c4a1e5e , 0x4362 , 0x45db , 0xb8d6 , 0xac71c6810066
		> , IStream >
	{
		void Open(SocketOsType sock);
		void Bind(sockaddr_in);
		void Bind6(sockaddr_in6);
		void GetSockName(sockaddr* name, int* namelen);
		void SetMembership(cppcomponents::cr_string multicast_addr, cppcomponents::cr_string interface_addr,
			std::uint32_t membership);
		void SetMulticastLoop(bool on);
		void SetMulticastTtl(std::int32_t ttl);
		void SetBroadcast(bool on);
		void SetTtl(std::int32_t ttl);
		use<IUdpSendRequest> Send(Buffer* bufs, int buffcnt, sockaddr_in addr,
			cppcomponents::use<UdpSendCallback> send_cb);
		use<IUdpSendRequest> Send6(Buffer* bufs, int buffcnt, sockaddr_in6 addr,
			cppcomponents::use<UdpSendCallback> send_cb);

		void RecvStart(cppcomponents::use<AllocCallback>, cppcomponents::use<RecvCallback>);
		void RecvStop();

		CPPCOMPONENTS_CONSTRUCT(IUdpStream, Open,Bind,Bind6,GetSockName,SetMembership);
	};

	struct IUdpStreamFactory
		: public cppcomponents::define_interface <
		cppcomponents::uuid
		< 0xc8f22d60 , 0xefa2 , 0x4d63 , 0xa6fc , 0x66d705f77352		>>
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> UdpInit(cppcomponents::use<ILoop>);

		CPPCOMPONENTS_CONSTRUCT(IUdpStreamFactory, UdpInit);
	};


	struct ITty
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0x87fa2ac3 , 0x2119 , 0x4aa7 , 0x98d1 , 0xf7bb86e0ae5e		>,
		IStream > 
	{
		void SetMode(int mode);
		void ResetMode();
		std::pair<int,int> GetWinsize();

		CPPCOMPONENTS_CONSTRUCT(IPipe, SetMode, ResetMode, GetWinsize);
		
	};

	struct ITtyFactory
		: public cppcomponents::define_interface <
		cppcomponents::uuid
		< 0xc8f22d60, 0xefa2, 0x4d63, 0xa6fc, 0x66d705f77352 >>
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> Init(cppcomponents::use<ILoop>,FileOsType fd, bool readable);


		CPPCOMPONENTS_CONSTRUCT(ITtyFactory, Init);
	};
	struct IPipe
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0xce484587 , 0x7aa3 , 0x477c , 0x9ad8 , 0xd86ca293babf>,
		IStream >
	{
		void Open(FileOsType file);
		void Bind(cppcomponents::cr_string name);
		use<IConnectRequest> Connect(cppcomponents::cr_string name, cppcomponents::use<ConnectCallback> cb);
		void PendingInstances(int count);

		CPPCOMPONENTS_CONSTRUCT(IPipe, Open, Bind, Connect, PendingInstances);

	};

	struct IPipeFactory
		: public cppcomponents::define_interface <
		cppcomponents::uuid
		< 0xa8175c7e , 0x65a4 , 0x4659 , 0x8648 , 0x64ff00e2fc4b	>>
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> Init(cppcomponents::use<ILoop>, bool ipc);


		CPPCOMPONENTS_CONSTRUCT(IPipeFactory, Init);
	};



	struct IPoll
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0xa7f582ac , 0x8b78 , 0x4988 , 0xad12 , 0x7074bbde7e27>,
		IHandle >
	{
		void Start(int events, cppcomponents::use<PollCallback>);
		void Stop();

		CPPCOMPONENTS_CONSTRUCT(IPoll, Start,Stop);

	};

	struct IPollFactory
		: public cppcomponents::define_interface <
		cppcomponents::uuid
		< 0x7f35a833 , 0xf555 , 0x4582 , 0x8e02 , 0x080a6c36f528		>>
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> Init(cppcomponents::use<ILoop>, FileOsType file);
		cppcomponents::use<cppcomponents::InterfaceUnknown> InitSocket(cppcomponents::use<ILoop>, SocketOsType socket);


		CPPCOMPONENTS_CONSTRUCT(IPollFactory, Init,InitSocket);
	};

	struct IPrepare
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0xc15bdb50 , 0x4fc6 , 0x4224 , 0x984d , 0x4a9e57f3d7ee		>,
		IHandle >
	{
		void Start(cppcomponents::use<PrepareCallback>);
		void Stop();

		CPPCOMPONENTS_CONSTRUCT(IPrepare, Start, Stop);
	};

	// DefaultFactory works for IPrepare Init
	
	struct ICheck
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0x313b4d56 , 0x1def , 0x431d , 0x84fb , 0x48556b725e20		>,
		IHandle >
	{
		void Start(cppcomponents::use<CheckCallback>);
		void Stop();

		CPPCOMPONENTS_CONSTRUCT(ICheck, Start, Stop);
	};
	struct IIdle
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0xdc2333a3 , 0xa2c8 , 0x47c4 , 0xbfde , 0xee062553c8df>,
		IHandle >
	{
		void Start(cppcomponents::use<IdleCallback>);
		void Stop();

		CPPCOMPONENTS_CONSTRUCT(IIdle, Start, Stop);
	};	

	struct IAsync
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0x5def2371 , 0x0da2 , 0x40de,0x88c4 , 0x1d7aeee7ec4c>,
		IHandle >
	{
		void Send();

		CPPCOMPONENTS_CONSTRUCT(IAsync,Send);
	};
	typedef cppcomponents::delegate<void(cppcomponents::use<IAsync>,int status),
		cppcomponents::uuid < 0x0971b76f , 0xced9 , 0x455c , 0xaf5f , 0x9cf128d5f04e>> AsyncCallback;

	struct IAsyncFactory
		: public cppcomponents::define_interface <
		cppcomponents::uuid <0x2d5db39d , 0x52df , 0x4900 , 0xa60f , 0x48c3fcc8b154	> >
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> Init(cppcomponents::use<ILoop>, cppcomponents::use<AsyncCallback>);

		CPPCOMPONENTS_CONSTRUCT(IAsyncFactory, Init);
	};

	// Note Async is the only Handle class that does not call Close in its destructor
	// This is because Async can be used on other threads and Close in another thread is unsafe


	struct ITimer
		: public cppcomponents::define_interface <
		cppcomponents::uuid<0x5f941871 , 0x8e16 , 0x4c21 , 0xbec6 , 0x33ee71c66afb>,
		IHandle >
	{
		void Start(cppcomponents::use<TimerCallback>,std::uint64_t timeout, std::uint64_t repeat);
		void Stop();
		void Again();
		void SetRepeat(std::uint64_t repeat);
		std::uint64_t GetRepeat();

		CPPCOMPONENTS_CONSTRUCT(ITimer,Start,Stop,Again,SetRepeat,GetRepeat);
	};


	struct ITimerFactory
		: public cppcomponents::define_interface <
		cppcomponents::uuid <0x52b4032d , 0x2898 , 0x4a5d , 0x944c , 0x9826fa0dddeb		> >
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> Init(cppcomponents::use<ILoop>, cppcomponents::use<AsyncCallback>);

		CPPCOMPONENTS_CONSTRUCT(IAsyncFactory, Init);
	};




	struct ICpuInfo 
		: public cppcomponents::define_interface < cppcomponents::uuid < 0x747f0089 , 0xea00 , 0x4821 , 0xa75c , 0xd64760ef98cc	>>
	{
		cppcomponents::cr_string GetModel();
		std::int32_t GetSpeed();
		std::int64_t GetUser();
		std::int64_t GetNice();
		std::int64_t GetSys();
		std::int64_t GetIdle();
		std::int64_t GetIrq();

		CPPCOMPONENTS_CONSTRUCT(ICpuInfo, GetModel, GetSpeed, GetUser, GetNice, GetSys, GetIdle, GetIrq);

	};

	struct IInterfaceAddress
		: public cppcomponents::define_interface < cppcomponents::uuid < 0x58aef2f1 , 0xa1b6 , 0x4a14 , 0xa417 , 0x9f1f95fed845	>>
	{
		cppcomponents::cr_string GetName();
		cppcomponents::cr_string GetPhysAddr();
		int IsInternal();
		sockaddr_in GetAddress4();
		sockaddr_in6 GetAddress6();
		sockaddr_in GetNetMask4();
		sockaddr_in6 GetNetMask6();

		CPPCOMPONENTS_CONSTRUCT(IInterfaceAddress, GetName, GetPhysAddr, IsInternal, GetAddress4, GetAddress6,
			GetNetMask4, GetNetMask6);
	};

	struct IUvStatics
		: cppcomponents::define_interface < cppcomponents::uuid < 0x92979519, 0x8fe7, 0x42cf, 0x8916, 0x6e6e66b46d4a	>>
	{
		int Version();
		cppcomponents::cr_string VersionString();
		cppcomponents::cr_string Strerror(int err);
		cppcomponents::cr_string ErrName(int err);
		std::size_t HandleSize(int type);
		std::size_t ReqestSize(int type);
		Buffer BufferInit(void* base, std::uint32_t len);
		std::size_t Strlcpy(char* dst, const char* src, std::size_t size);
		std::size_t Strlcat(char* dst, const char* src, std::size_t size);
		int GuessHandle(FileOsType file);

		void Getaddrinfo(cppcomponents::use<ILoop>, cppcomponents::use<GetaddrinfoCallback>, cppcomponents::cr_string node,
			cppcomponents::cr_string service, addrinfo* hints);

		void Freeaddrinfo(addrinfo* ai);

		char** SetupArgs(int argc, char** argv);

		cppcomponents::cr_string GetProcessTitle();
		void SetProcessTitle(cppcomponents::cr_string);
		std::size_t ResidentSetMemory();
		double Uptime();

		std::vector<cppcomponents::use<ICpuInfo>> CpuInfo();
		std::vector<cppcomponents::use<IInterfaceAddress>> InterfaceAddresses();


		CPPCOMPONENTS_CONSTRUCT(IUvStatics, Version, VersionString,
			Strerror, ErrName, HandleSize, RequestSize, BufferInit, Strlcpy, Strlcat, GuessHandle,
			Getaddrinfo,Freeaddrinfo,SetupArgs,GetProcessTitle,SetProcessTitle,ResidentSetMemory,Uptime,
			CpuInfo,InterfaceAddresses);

	};

	inline std::string UvId(){ return "cppcomponents_lib_uv_dll!Uv"; }

	typedef cppcomponents::runtime_class<UvId, cppcomponents::static_interfaces<IUvStatics>> Uv_t;
	typedef cppcomponents::use_runtime_class<Uv_t> Uv;

	typedef cppcomponents::delegate < void(int exit_status, int term_signal),
		cppcomponents::uuid<0xa8b1fddf , 0x1501 , 0x4711 , 0x8ef1 , 0x9ae7ebe300a9	> > ExitCallback;

	struct IStdioContainer
		: public cppcomponents::define_interface <
		cppcomponents::uuid < 0x6afae11e, 0x9c4c, 0x449a, 0xab0f, 0xfa14f085464f> >
	{
		int GetFlags();
		void SetFlags(int flags);
		bool IsStream();
		void SetStream(cppcomponents::use<IStream>);
		cppcomponents::use<IStream> GetStream();
		void SetFd(FileOsType fd);
		FileOsType GetFd();

		CPPCOMPONENTS_CONSTRUCT(IStdioContainer, GetFlags, SetFlags, IsStream, SetStream, GetStream, SetFd, GetFd);


	};

	struct IProcessOptions
		: public cppcomponents::define_interface < cppcomponents::uuid < 0x18b90130 , 0x2ea2 , 0x4c21 , 0x934b , 0xf79cf31c7242	>>
	{
		void SetExitCallback(cppcomponents::use<ExitCallback>);
		cppcomponents::use<ExitCallback> GetExitCallback();

		void SetFile(cppcomponents::cr_string file);
		cppcomponents::cr_string GetFile();

		void SetArgs(std::vector<cppcomponents::cr_string> args);
		std::vector<cppcomponents::cr_string> GetArgs();

		void SetEnv(std::vector<cppcomponents::cr_string> args);
		std::vector<cppcomponents::cr_string> GetEnv();

		void SetCwd(cppcomponents::cr_string file);
		cppcomponents::cr_string GetCwd();		

		unsigned int GetFlags();
		void SetFlags(unsigned int flags);

		void SetStdio(cppcomponents::use<IStdioContainer>);
		cppcomponents::use<IStdioContainer> GetStdio();

		void SetUid(unsigned char);
		unsigned char GetUid();

		void SetGid(unsigned char);
		unsigned char GetGid();


		CPPCOMPONENTS_CONSTRUCT(IProcessOptions, SetExitCallback, GetExitCallback, SetFile, GetFile,
			SetArgs, GetArgs, SetEnv, GetEnv, SetCwd, GetCwd, GetFlags, SetFlags, SetStdio, GetStdio, SetUid, GetUid,
			SetGid, GetGid);

	};

	struct IProcess 
		:public cppcomponents::define_interface < 
		cppcomponents::uuid<0xee9c7f5b , 0x3be6 , 0x4a40 , 0xa689 , 0xa4bd7bbc4f4d	>,
		IHandle>
	{
		void Kill(int signum);
		CPPCOMPONENTS_CONSTRUCT(IProcess, Kill);
	};

	struct IProcessFactory
		: public cppcomponents::define_interface<
		cppcomponents::uuid<0xff1b1982, 0x6bd6, 0x4096, 0xaf0d, 0xf6488421cc64	> >
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> Spawn(cppcomponents::use<ILoop>, cppcomponents::use<IProcessOptions>);

		CPPCOMPONENTS_CONSTRUCT(IProcessFactory, Spawn);
	};
	struct IProcessStatics
		: public cppcomponents::define_interface<
		cppcomponents::uuid<0x66fdeeba , 0x6b9f , 0x4003 , 0x8fc1 , 0x8192d21488cf	> >
	{
		cppcomponents::use<cppcomponents::InterfaceUnknown> Kill(int pid, int signum);

		CPPCOMPONENTS_CONSTRUCT(IProcessStatics, Kill);
	};




	struct IFsStatics
		: public cppcomponent::define_interface <
		cppcomponents::uuid<0x96340307, 0x442f, 0x4327, 0x8fd0, 0xcbb71d95bf8a>
		>
	{
		use<IFsRequest> Close(use<ILoop>, FileOsType, use<FsCallback>);
		use<IFsRequest> Open(use<ILoop>, cppcomponents::cr_string, int flags,
			int mode, use<FsCallback>);



	};



}

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

	T get()const{
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