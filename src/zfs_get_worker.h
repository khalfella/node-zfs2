#ifndef __ZFS_GET_WORKER__
#define __ZFS_GET_WORKER__


#include <nan.h>
#include <map>
#include <mutex>
#include <libzfs.h>

using namespace v8;
using namespace Nan;

class ZFSGetWorker : public AsyncWorker {
	public:
		ZFSGetWorker(Nan::Callback *callback, std::string name, libzfs_handle_t* lzfsh, std::mutex *lzfsLock);
		void Execute();
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		zfs_handle_t *zfsh;
                libzfs_handle_t *lzfsh;
		std::mutex *lzfsLock;
		std::map<std::string, std::string> string_props;
		std::map<std::string, double> numeric_props;
};

#endif // __ZFS_GET_WORKER__
