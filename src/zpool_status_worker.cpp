#include <nan.h>

#include "zpool_status_worker.h"

using namespace Nan;
using namespace v8;

ZPoolStatusWorker::ZPoolStatusWorker(Nan::Callback *callback, std::string name) : ZFSWorker(callback) {
	this->name = name;
	this->errorMessage = "";
}


void ZPoolStatusWorker::Run(libzfs_handle_t *lzfsh) {
	zpool_handle_t *zph;

	if (lzfsh == NULL) {
		this->errorMessage = "Error initializing libzfs";
		return;
	}
	if ((zph = zpool_open(lzfsh, this->name.c_str())) == NULL) {
		this->errorMessage = "Filed to open: " + this->name;
		return;
	}

	char buf[ZFS_MAXPROPLEN];
	if (zpool_get_prop(zph, ZPOOL_PROP_HEALTH, buf, sizeof(buf), NULL, _B_TRUE) != 0) {
		this->errorMessage = "Filed to get pool health state: " + this->name;
		zpool_close(zph);
		return;
	}

	this->state = buf;
	zpool_close(zph);
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
