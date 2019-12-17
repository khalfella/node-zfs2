#ifndef __ZPOOL_STATUS_WORKER__
#define __ZPOOL_STATUS_WORKER__

#include <nan.h>
#include <mutex>

#include "zfs_worker.h"

using namespace v8;
using namespace Nan;

class ZPoolStatusWorker : public ZFSWorker {
	public:
		ZPoolStatusWorker(Nan::Callback *, std::string);
		void Run(ZFSWorkerLibZFSHandle *);
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		std::string state;
};

#endif // __ZPOOL_STATUS_WORKER__
