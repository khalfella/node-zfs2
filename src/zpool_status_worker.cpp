#include <nan.h>
#include <libzfs.h>

#include "zpool_status_worker.h"

using namespace Nan;
using namespace v8;

ZPoolStatusWorker::ZPoolStatusWorker(Nan::Callback *callback, std::string name) :
AsyncWorker(callback) {
	this->name = name;
	this->errorMessage = "";
}


void ZPoolStatusWorker::Execute() {
	libzfs_handle_t *lh;
	zpool_handle_t *zph;

	if ((lh = libzfs_init()) == NULL) {
		this->errorMessage = "error initialized libzfs";
		return;
	}
	if ((zph = zpool_open(lh, this->name.c_str())) == NULL) {
		this->errorMessage = "filed to open the pool";
		libzfs_fini(lh);
		return;
	}

	char buf[ZFS_MAXPROPLEN];
	if (zpool_get_prop(zph, ZPOOL_PROP_HEALTH, buf, sizeof(buf), NULL, _B_TRUE) != 0) {
		this->errorMessage = "filed to get pool health state";
		zpool_close(zph);
		libzfs_fini(lh);
		return;
	}

	this->state = buf;

	zpool_close(zph);
	libzfs_fini(lh);
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
