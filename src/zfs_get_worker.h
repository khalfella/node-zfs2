#ifndef __ZFS_GET_WORKER__
#define __ZFS_GET_WORKER__


#include <nan.h>
#include <map>
#include <mutex>

#include "zfs_worker.h"

using namespace v8;
using namespace Nan;

class ZFSGetWorker : public ZFSWorker {
	public:
		ZFSGetWorker(Nan::Callback *, std::string);
		void Run(ZFSWorkerLibZFSHandle *);
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		ZFSWorkerZFSHandle *zfsh;
		std::vector<ZFSProperty> props;
};

#endif // __ZFS_GET_WORKER__
