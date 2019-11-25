#ifndef __ZFS_GET_WORKER__
#define __ZFS_GET_WORKER__


#include <nan.h>
#include <map>
#include <mutex>
#include <libzfs.h>

#include "zfs_worker.h"

using namespace v8;
using namespace Nan;

class ZFSGetWorker : public ZFSWorker {
	public:
		ZFSGetWorker(Nan::Callback *callback, std::string name);
		void Run(libzfs_handle_t *);
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		zfs_handle_t *zfsh;
		std::map<std::string, std::string> string_props;
		std::map<std::string, double> numeric_props;
};

#endif // __ZFS_GET_WORKER__
