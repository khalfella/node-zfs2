#include <nan.h>
#include <iostream>
#include <libzfs.h>
#include <map>

#include "zfs_get_worker.h"

using namespace Nan;
using namespace v8;

ZFSGetWorker::ZFSGetWorker(Nan::Callback *callback, std::string name) :
AsyncWorker(callback) {
	this->name = name;
	this->errorMessage = "";
}


void ZFSGetWorker::Execute() {
	libzfs_handle_t *lh;

	if ((lh = libzfs_init()) == NULL) {
		this->errorMessage = "error initialized libzfs";
		return;
	}
	if ((this->zfsh = zfs_open(lh, this->name.c_str(), ZFS_TYPE_DATASET)) == NULL) {
		this->errorMessage = "error initialized libzfs";
		libzfs_fini(lh);
		return;
	}

	auto cb = [](int p, void *data)  {
		auto *self = (ZFSGetWorker*) data;
		auto prop = (zfs_prop_t) p;

		char buf[ZFS_MAXPROPLEN];
		if (zfs_prop_get(self->zfsh, prop, buf, sizeof(buf), NULL, NULL, 0, _B_TRUE) == 0) {
			if (zfs_prop_is_string(prop)) {
				self->string_props.insert(std::pair<std::string, std::string>(zfs_prop_to_name(prop), buf));
			} else {
				self->numeric_props.insert(std::pair<std::string, double>(zfs_prop_to_name( prop), std::stod(buf)));
			}
		}

		return ((int)ZPROP_CONT);
	};

	zprop_iter(cb, this, _B_TRUE, _B_TRUE, (zfs_type_t) ZFS_TYPE_DATASET);

	zfs_close(zfsh);
	libzfs_fini(lh);
}

void ZFSGetWorker::HandleOKCallback() {

	if (this->errorMessage.length() > 0) {
		Local<Value> argv[] = {
			Nan::New<String>(this->errorMessage.c_str())
			    .ToLocalChecked()
		};
		callback->Call(1, argv, async_resource);
		return;
	}
	Local<Object> props = Nan::New<Object>();

	for (auto itr = this->string_props.begin(); itr != this->string_props.end(); ++itr) {
		Local<String> key = Nan::New<String>(itr->first).ToLocalChecked();
		Nan::Set(props,  key, Nan::New<String>(itr->second).ToLocalChecked());
	}

	for (auto itr = this->numeric_props.begin(); itr != this->numeric_props.end(); ++itr) {
		Local<String> key = Nan::New<String>(itr->first).ToLocalChecked();
		Nan::Set(props,  key, Nan::New<Number>(itr->second));
	}
	Local<Value> argv[] = { Nan::Null(), props};
	callback->Call(2, argv, async_resource);
}
