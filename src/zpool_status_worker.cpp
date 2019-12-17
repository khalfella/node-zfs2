#include <nan.h>

#include "zpool_status_worker.h"

using namespace Nan;
using namespace v8;

ZPoolStatusWorker::ZPoolStatusWorker(Nan::Callback *callback, std::string name) : ZFSWorker(callback) {
	this->name = name;
	this->errorMessage = "";
}


void ZPoolStatusWorker::Run(libzfs_handle_t *lzfsh) {
	ZFSWorkerZPoolHandle *zph;

	if (lzfsh == NULL) {
		this->errorMessage = "Error initializing libzfs";
		return;
	}
	if ((zph = ZFSWorker::ZPoolOpen(lzfsh, this->name.c_str())) == NULL) {
		this->errorMessage = "Filed to open: " + this->name;
		return;
	}

	char buf[ZFSWorkerZFSMaxPropLen];
	if (ZFSWorker::ZPoolGetProp(zph, ZFSWorkerZPoolPropHealth,
	    buf, sizeof(buf), NULL, true) != 0) {

		this->errorMessage = "Filed to get pool health state: " + this->name;
		ZFSWorker::ZPoolClose(zph);
		return;
	}

	this->state = buf;
	ZFSWorker::ZPoolClose(zph);
}

void ZPoolStatusWorker::HandleOKCallback() {

	if (this->errorMessage.length() > 0) {
		Local<Value> argv[] = {
			Nan::New<String>(this->errorMessage.c_str())
			    .ToLocalChecked()
		};
		callback->Call(1, argv, async_resource);
		return;
	}
	Local<String> state = Nan::New<String>(this->state.c_str()).ToLocalChecked();
	Local<Value> argv[] = { Nan::Null(), state };
	callback->Call(2, argv, async_resource);
}
