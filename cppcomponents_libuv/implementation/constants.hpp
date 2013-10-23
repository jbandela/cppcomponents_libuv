namespace cppcomponents_libuv{
	namespace Constants{
	namespace Membership{
		enum {
			LeaveGroup = 0,
			JoinGroup
		};
	}

	namespace UdpFlags{
	enum  {
		/* Disables dual stack mode. Used with uv_udp_bind6(). */
		Ipv6only = 1,
		/*
		* Indicates message was truncated because read buffer was too small. The
		* remainder was discarded by the OS. Used in uv_udp_recv_cb.
		*/
		Partial = 2
	};
	}
	namespace PollEvent{
	enum  {
		Readable = 1,
		Writable = 2
	};
	}
	namespace StdioFlags{
		enum {
			Ignore = 0x00,
			CreatePipe = 0x01,
			InheritFd = 0x02,
			InheritStream = 0x04,

			/* When UV_CREATE_PIPE is specified, UV_READABLE_PIPE and UV_WRITABLE_PIPE
			* determine the direction of flow, from the child process' perspective. Both
			* flags may be specified to create a duplex data stream.
			*/
			ReadablePipe = 0x10,
			WritablePipe = 0x20
		};
	}
	
	namespace ProcessFlags{
	enum  {
		/*
		* Set the child process' user id. The user id is supplied in the `uid` field
		* of the options struct. This does not work on windows; setting this flag
		* will cause uv_spawn() to fail.
		*/
		Setuid = (1 << 0),
		/*
		* Set the child process' group id. The user id is supplied in the `gid`
		* field of the options struct. This does not work on windows; setting this
		* flag will cause uv_spawn() to fail.
		*/
		Setgid = (1 << 1),
		/*
		* Do not wrap any arguments in quotes, or perform any other escaping, when
		* converting the argument list into a command line string. This option is
		* only meaningful on Windows systems. On unix it is silently ignored.
		*/
		WindowsVerbatimArguments = (1 << 2),
		/*
		* Spawn the child process in a detached state - this will make it a process
		* group leader, and will effectively enable the child to keep running after
		* the parent exits.  Note that the child process will still keep the
		* parent's event loop alive unless the parent process calls uv_unref() on
		* the child's process handle.
		*/
		Detached = (1 << 3),
		/*
		* Hide the subprocess console window that would normally be created. This
		* option is only meaningful on Windows systems. On unix it is silently
		* ignored.
		*/
		WindowsHide = (1 << 4)
	};
	}

	namespace FsType{
	enum {
		Unknown = -1,
		Custom,
		Open,
		Close,
		Read,
		Write,
		Sendfile,
		Stat,
		Lstat,
		Fstat,
		Ftruncate,
		Utime,
		Futime,
		Chmod,
		Fchmod,
		Fsync,
		Fdatasync,
		Unlink,
		Rmdir,
		Mkdir,
		Rename,
		Readdir,
		Link,
		Symlink,
		Readlink,
		Chown,
		Fchown
	};
	}
	namespace FsEvent{
	enum  {
		Rename = 1,
		Change = 2
	};
	}
	namespace FsEventFlags{
	enum {
		/*
		* By default, if the fs event watcher is given a directory name, we will
		* watch for all events in that directory. This flags overrides this behavior
		* and makes fs_event report only changes to the directory entry itself. This
		* flag does not affect individual files watched.
		* This flag is currently not implemented yet on any backend.
		*/
		WatchEntry = 1,

		/*
		* By default uv_fs_event will try to use a kernel interface such as inotify
		* or kqueue to detect events. This may not work on remote filesystems such
		* as NFS mounts. This flag makes fs_event fall back to calling stat() on a
		* regular interval.
		* This flag is currently not implemented yet on any backend.
		*/
		Stat = 2,

		/*
		* By default, event watcher, when watching directory, is not registering
		* (is ignoring) changes in it's subdirectories.
		* This flag will override this behaviour on platforms that support it.
		*/
		Recursive = 3
	};
	}

	namespace HandleTypes{
//#define UV_HANDLE_TYPE_MAP(XX)                                                \
//	XX(ASYNC, async)                                                            \
//	XX(CHECK, check)                                                            \
//	XX(FS_EVENT, fs_event)                                                      \
//	XX(FS_POLL, fs_poll)                                                        \
//	XX(HANDLE, handle)                                                          \
//	XX(IDLE, idle)                                                              \
//	XX(NAMED_PIPE, pipe)                                                        \
//	XX(POLL, poll)                                                              \
//	XX(PREPARE, prepare)                                                        \
//	XX(PROCESS, process)                                                        \
//	XX(STREAM, stream)                                                          \
//	XX(TCP, tcp)                                                                \
//	XX(TIMER, timer)                                                            \
//	XX(TTY, tty)                                                                \
//	XX(UDP, udp)                                                                \
//	XX(SIGNAL, signal)


//		typedef enum {
//			UV_UNKNOWN_HANDLE = 0,
//#define XX(uc, lc) UV_##uc,
//			UV_HANDLE_TYPE_MAP(XX)
//#undef XX
//			UV_FILE,
//			UV_HANDLE_TYPE_MAX
//		} uv_handle_type;

		enum{
			Unknown = 0,
			Async,
			Check,
			FsEvent,
			FsPoll,
			Handle,
			Idle,
			NamedPipe,
			Poll,
			Prepare,
			Process,
			Stream,
			Tcp,
			Timer,
			Tty,
			Udp,
			Signal,
			File,
			TypeMax
		};
	}

	}
}