#ifndef __ZPOOL_STATUS_WORKER__
#define __ZPOOL_STATUS_WORKER__

#include <nan.h>
#include <libzfs.h>
#include <mutex>

using namespace v8;
using namespace Nan;

class ZPoolStatusWorker : public AsyncWorker {
	public:
		ZPoolStatusWorker(Nan::Callback *callback, std::string name, libzfs_handle_t *lzfsh, std::mutex*);
		void Execute();
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		std::string state;
		libzfs_handle_t *lzfsh;
		std::mutex* lzfsLock;
};

#endif // __ZPOOL_STATUS_WORKER__
