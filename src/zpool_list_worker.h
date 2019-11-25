#ifndef __ZPOOL_LIST_WORKER__
#define __ZPOOL_LIST_WORKER__

#include <nan.h>
#include <libzfs.h>
#include <mutex>

#include "zfs_worker.h"

using namespace v8;
using namespace Nan;

class ZPoolListWorker : public ZFSWorker {
	public:
		ZPoolListWorker(Nan::Callback *callback);
		void Run(libzfs_handle_t *);
		void HandleOKCallback();
	private:
		std::string				errorMessage;
		std::vector<std::vector<ZFSProperty>>	props;
		std::vector<ZFSProperty>*		cur_zprops;
		zpool_handle_t*				cur_zph;
};

#endif // __ZPOOL_LIST_WORKER__
