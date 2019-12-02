#ifndef __ZFS_WORKER__
#define __ZFS_WORKER__

#include <nan.h>
#include <mutex>
#include <libzfs.h>

using namespace v8;
using namespace Nan;

class ZFSProperty {
	public:
		bool MayBeNumericZFSProperty(double *);
		bool MayBeNumericZpoolProperty(double *);
	public:
		int		prop;
		std::string	name;
		std::string	value;
		zprop_source_t	source;
		std::string	where;
};

class ZFSWorker : public AsyncWorker {
	public:
		ZFSWorker(Nan::Callback *);
		void Execute();
		virtual void Run(libzfs_handle_t *);
		static void InitializeLibZFS();
	private:
                static libzfs_handle_t *lzfsh;
		static std::mutex lzfsLock;
};


#endif // __ZFS_WORKER__
