#include <errno.h>


// Copied and modified from uv-errno.h

namespace cppcomponents_libuv{
	namespace ErrorCodes{
	enum {
		Eof = (-4095),
			Unknown = (-4094),

			Addrfamily = (-3000),
			Again = (-3001),
			Badflag = (-3002),
			Canceled = (-3003),
			Fail = (-3004),
			Family = (-3005),
			Memory = (-3006),
			Nodata = (-3007),
			Noname = (-3008),
			Overflow = (-3009),
			Service = (-3010),
			Socktype = (-3011),
			System = (-3012),

#if defined(EACCES) && !defined(_WIN32)
			Acces = (-EACCES),
#else
			Acces = (-4093),
#endif

#if defined(EADDRINFO) && !defined(_WIN32)
			Addrinfo = EADDRINFO,
#else
			Addrinfo = (-4092),
#endif

#if defined(EADDRINUSE) && !defined(_WIN32)
			Addrinuse = (-EADDRINUSE),
#else
			Addrinuse = (-4092),
#endif

#if defined(EADDRNOTAVAIL) && !defined(_WIN32)
			Addrnotavail = (-EADDRNOTAVAIL),
#else
			Addrnotavail = (-4091),
#endif

#if defined(EAFNOSUPPORT) && !defined(_WIN32)
			Afnosupport = (-EAFNOSUPPORT),
#else
			Afnosupport = (-4090),
#endif

#if defined(EAGAIN) && !defined(_WIN32)
			EAgain = (-EAGAIN),
#else
			EAgain = (-4089),
#endif

#if defined(EALREADY) && !defined(_WIN32)
			Already = (-EALREADY),
#else
			Already = (-4085),
#endif

#if defined(EBADF) && !defined(_WIN32)
			Badf = (-EBADF),
#else
			Badf = (-4084),
#endif

#if defined(EBUSY) && !defined(_WIN32)
			Busy = (-EBUSY),
#else
			Busy = (-4083),
#endif

#if defined(ECANCELED) && !defined(_WIN32)
			ECanceled = (-ECANCELED),
#else
			ECanceled = (-4082),
#endif

#if defined(ECHARSET) && !defined(_WIN32)
			Charset = (-ECHARSET),
#else
			Charset = (-4081),
#endif

#if defined(ECONNABORTED) && !defined(_WIN32)
			Connaborted = (-ECONNABORTED),
#else
			Connaborted = (-4080),
#endif

#if defined(ECONNREFUSED) && !defined(_WIN32)
			Connrefused = (-ECONNREFUSED),
#else
			Connrefused = (-4079),
#endif

#if defined(ECONNRESET) && !defined(_WIN32)
			Connreset = (-ECONNRESET),
#else
			Connreset = (-4078),
#endif

#if defined(EDESTADDRREQ) && !defined(_WIN32)
			Destaddrreq = (-EDESTADDRREQ),
#else
			Destaddrreq = (-4077),
#endif

#if defined(EEXIST) && !defined(_WIN32)
			Exist = (-EEXIST),
#else
			Exist = (-4076),
#endif

#if defined(EFAULT) && !defined(_WIN32)
			Fault = (-EFAULT),
#else
			Fault = (-4075),
#endif

#if defined(EHOSTUNREACH) && !defined(_WIN32)
			Hostunreach = (-EHOSTUNREACH),
#else
			Hostunreach = (-4074),
#endif

#if defined(EINTR) && !defined(_WIN32)
			Intr = (-EINTR),
#else
			Intr = (-4073),
#endif

#if defined(EINVAL) && !defined(_WIN32)
			Inval = (-EINVAL),
#else
			Inval = (-4072),
#endif

#if defined(EIO) && !defined(_WIN32)
			Io = (-EIO),
#else
			Io = (-4071),
#endif

#if defined(EISCONN) && !defined(_WIN32)
			Isconn = (-EISCONN),
#else
			Isconn = (-4070),
#endif

#if defined(EISDIR) && !defined(_WIN32)
			Isdir = (-EISDIR),
#else
			Isdir = (-4069),
#endif

#if defined(ELOOP) && !defined(_WIN32)
			Loop = (-ELOOP),
#else
			Loop = (-4068),
#endif

#if defined(EMFILE) && !defined(_WIN32)
			Mfile = (-EMFILE),
#else
			Mfile = (-4067),
#endif

#if defined(EMSGSIZE) && !defined(_WIN32)
			Msgsize = (-EMSGSIZE),
#else
			Msgsize = (-4066),
#endif

#if defined(ENAMETOOLONG) && !defined(_WIN32)
			Nametoolong = (-ENAMETOOLONG),
#else
			Nametoolong = (-4065),
#endif

#if defined(ENETDOWN) && !defined(_WIN32)
			Netdown = (-ENETDOWN),
#else
			Netdown = (-4064),
#endif

#if defined(ENETUNREACH) && !defined(_WIN32)
			Netunreach = (-ENETUNREACH),
#else
			Netunreach = (-4063),
#endif

#if defined(ENFILE) && !defined(_WIN32)
			Nfile = (-ENFILE),
#else
			Nfile = (-4062),
#endif

#if defined(ENOBUFS) && !defined(_WIN32)
			Nobufs = (-ENOBUFS),
#else
			Nobufs = (-4061),
#endif

#if defined(ENODEV) && !defined(_WIN32)
			Nodev = (-ENODEV),
#else
			Nodev = (-4060),
#endif

#if defined(ENOENT) && !defined(_WIN32)
			Noent = (-ENOENT),
#else
			Noent = (-4059),
#endif

#if defined(ENOMEM) && !defined(_WIN32)
			Nomem = (-ENOMEM),
#else
			Nomem = (-4058),
#endif

#if defined(ENONET) && !defined(_WIN32)
			Nonet = (-ENONET),
#else
			Nonet = (-4057),
#endif

#if defined(ENOSPC) && !defined(_WIN32)
			Nospc = (-ENOSPC),
#else
			Nospc = (-4056),
#endif

#if defined(ENOSYS) && !defined(_WIN32)
			Nosys = (-ENOSYS),
#else
			Nosys = (-4055),
#endif

#if defined(ENOTCONN) && !defined(_WIN32)
			Notconn = (-ENOTCONN),
#else
			Notconn = (-4054),
#endif

#if defined(ENOTDIR) && !defined(_WIN32)
			Notdir = (-ENOTDIR),
#else
			Notdir = (-4053),
#endif

#if defined(ENOTEMPTY) && !defined(_WIN32)
			Notempty = (-ENOTEMPTY),
#else
			Notempty = (-4052),
#endif

#if defined(ENOTSOCK) && !defined(_WIN32)
			Notsock = (-ENOTSOCK),
#else
			Notsock = (-4051),
#endif

#if defined(ENOTSUP) && !defined(_WIN32)
			Notsup = (-ENOTSUP),
#else
			Notsup = (-4050),
#endif

#if defined(EPERM) && !defined(_WIN32)
			Perm = (-EPERM),
#else
			Perm = (-4049),
#endif

#if defined(EPIPE) && !defined(_WIN32)
			Pipe = (-EPIPE),
#else
			Pipe = (-4048),
#endif

#if defined(EPROTO) && !defined(_WIN32)
			Proto = (-EPROTO),
#else
			Proto = (-4047),
#endif

#if defined(EPROTONOSUPPORT) && !defined(_WIN32)
			Protonosupport = (-EPROTONOSUPPORT),
#else
			Protonosupport = (-4046),
#endif

#if defined(EPROTOTYPE) && !defined(_WIN32)
			Prototype = (-EPROTOTYPE),
#else
			Prototype = (-4045),
#endif

#if defined(EROFS) && !defined(_WIN32)
			Rofs = (-EROFS),
#else
			Rofs = (-4044),
#endif

#if defined(ESHUTDOWN) && !defined(_WIN32)
			Shutdown = (-ESHUTDOWN),
#else
			Shutdown = (-4043),
#endif

#if defined(ESPIPE) && !defined(_WIN32)
			Spipe = (-ESPIPE),
#else
			Spipe = (-4042),
#endif

#if defined(ESRCH) && !defined(_WIN32)
			Srch = (-ESRCH),
#else
			Srch = (-4041),
#endif

#if defined(ETIMEDOUT) && !defined(_WIN32)
			Timedout = (-ETIMEDOUT),
#else
			Timedout = (-4040),
#endif

#if defined(EXDEV) && !defined(_WIN32)
			Xdev = (-EXDEV),
#else
			Xdev = (-4038),
#endif


	};

}
}
/* Only map to the system errno on non-Windows platforms. It's apparently
* a fairly common practice for Windows programmers to redefine errno codes.
*/
