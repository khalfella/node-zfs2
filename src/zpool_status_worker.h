#ifndef __ZPOOL_STATUS_WORKER__
#define __ZPOOL_STATUS_WORKER__

#include <nan.h>
#include <libzfs.h>

using namespace v8;
using namespace Nan;

class ZPoolStatusWorker : public AsyncWorker {
	public:
		ZPoolStatusWorker(Nan::Callback *callback, std::string name);
		void Execute();
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		std::string state;
};

#endif // __ZPOOL_STATUS_WORKER__
