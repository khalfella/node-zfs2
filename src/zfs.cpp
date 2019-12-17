#include <nan.h>
#include <iostream>
#include <libzfs.h>
#include <map>
#include <mutex>

#include "zfs_get_worker.h"
#include "zpool_status_worker.h"
#include "zpool_list_worker.h"
#include "zfs_worker.h"

using namespace Nan;
using namespace v8;


NAN_METHOD(zfsGet) {
	Local<Object> opts;
	if (info.Length() < 2 ||
	    !Nan::To<Object>(info[0]).ToLocal(&opts) ||
	    !info[1]->IsFunction()) {

		Nan::ThrowError("zfsGet(): Invalid input arguments");
		return;
	}

        Local<String> name_prop = Nan::New<String>("name").ToLocalChecked();

	Local<v8::Value> name;
        if (Nan::Get(opts, name_prop).ToLocal(&name) &&  name->IsString()) {
		Nan::Callback *cb = new Nan::Callback(To<Function>(info[1])
		    .ToLocalChecked());
		auto dsname = Nan::To<v8::String>(name).ToLocalChecked();
		AsyncQueueWorker(new ZFSGetWorker(cb, *Nan::Utf8String(dsname)));
	} else {
		Nan::ThrowError("zfsGet(): Invalid input arguments");
	}
}

NAN_METHOD(zpoolStatus) {
	Local<Object> opts;
	if (info.Length() < 2 ||
	    !Nan::To<Object>(info[0]).ToLocal(&opts) ||
	    !info[1]->IsFunction()) {
		Nan::ThrowError("zpoolStatus(): Invalid input arguments");
		return;
	}

	Local<String> name_prop = Nan::New<String>("name").ToLocalChecked();

	Local<v8::Value> name;
	if (Nan::Get(opts, name_prop).ToLocal(&name) && name->IsString()) {
		Nan::Callback *cb = new Nan::Callback(To<Function>(info[1])
		    .ToLocalChecked());
		auto pname = Nan::To<v8::String>(name).ToLocalChecked();
		AsyncQueueWorker(new ZPoolStatusWorker(cb, *Nan::Utf8String(pname)));
	} else {
		Nan::ThrowError("zpoolStatus(): Invalid input arguments");
	}
}

NAN_METHOD(zpoolList) {
	// The first argument is not used. It is reserved for future use.
	if (info.Length() < 2 || !info[0]->IsObject() || !info[1]->IsFunction()) {
		Nan::ThrowError("zpoolStatus(): Invalid input arguments");
		return;
	}

	Nan::Callback *cb = new Nan::Callback(To<Function>(info[1]).ToLocalChecked());
	AsyncQueueWorker(new ZPoolListWorker(cb));
}

NAN_MODULE_INIT(Init) {
	ZFSWorker::InitializeLibZFS();

	Nan::Set(target,
	    New<String>("zfsGet").ToLocalChecked(),
	    GetFunction(New<FunctionTemplate>(zfsGet))
	        .ToLocalChecked());

	Nan::Set(target,
	    New<String>("zpoolStatus").ToLocalChecked(),
	    GetFunction(New<FunctionTemplate>(zpoolStatus))
	    .ToLocalChecked());
	Nan::Set(target,
	    New<String>("zpoolList").ToLocalChecked(),
	    GetFunction(New<FunctionTemplate>(zpoolList))
	    .ToLocalChecked());
}

NODE_MODULE(zfs2, Init)
