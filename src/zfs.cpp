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
	if (info.Length() < 2 || !info[0]->IsObject() || !info[1]->IsFunction()) {
		return;
	}

	Local<Object> opts = info[0]->ToObject();
        Local<String> name_prop = Nan::New<String>("name").ToLocalChecked();

	Local<v8::Value> name;
        if (Nan::Get(opts, name_prop).ToLocal(&name) && name->IsString()) {
		Nan::Callback *cb = new Nan::Callback(To<Function>(info[1])
		    .ToLocalChecked());
		v8::String::Utf8Value str(name->ToString());
		std::string s(*str, str.length());
		AsyncQueueWorker(new ZFSGetWorker(cb, s));
	}
}

NAN_METHOD(zpoolStatus) {
	if (info.Length() < 2 || !info[0]->IsObject() || !info[1]->IsFunction()) {
		return;
	}

	Local<Object> opts = info[0]->ToObject();
	Local<String> name_prop = Nan::New<String>("name").ToLocalChecked();

	Local<v8::Value> name;
	if (Nan::Get(opts, name_prop).ToLocal(&name) && name->IsString()) {
		Nan::Callback *cb = new Nan::Callback(To<Function>(info[1])
		    .ToLocalChecked());
		v8::String::Utf8Value str(name->ToString());
		std::string s(*str, str.length());
		AsyncQueueWorker(new ZPoolStatusWorker(cb, s));
	}
}

NAN_METHOD(zpoolList) {
	if (info.Length() != 1 || !info[0]->IsFunction()) {
		// should return an error in this case or do something.
		return;
	}

	Nan::Callback *cb = new Nan::Callback(To<Function>(info[0]).ToLocalChecked());
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
