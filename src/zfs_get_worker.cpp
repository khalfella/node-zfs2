#include <nan.h>
#include <libzfs.h>
#include <iostream>

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
		zprop_source_t source;
		char where[ZFS_MAXPROPLEN];
		strcpy(where, "");

		if (zfs_prop_get(self->zfsh, prop, buf, sizeof(buf), &source, where, sizeof(where), _B_TRUE) == 0) {

			__zfs_prop__ p;
			p.name = zfs_prop_to_name(prop);
			p.is_num = zfs_prop_is_string(prop);
			p.value = buf;
			p.source = source;
			p.where = where;
			self->props.push_back(p);
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

	for (auto itr = this->props.begin(); itr != this->props.end(); ++itr) {
		Local<String> key = Nan::New<String>(itr->name).ToLocalChecked();

		Local<Object> value = Nan::New<Object>();
		Nan::Set(value,
		    Nan::New<String>("isNumeric").ToLocalChecked(),
		    Nan::New<String>("yes").ToLocalChecked());
		Nan::Set(value,
		    Nan::New<String>("value").ToLocalChecked(),
		    Nan::New<String>(itr->value.c_str()).ToLocalChecked());
		Nan::Set(value,
		    Nan::New<String>("source").ToLocalChecked(),
		    Nan::New<String>(itr->source.c_str()).ToLocalChecked());
		Nan::Set(value,
		    Nan::New<String>("where").ToLocalChecked(),
		    Nan::New<String>(itr->where.c_str()).ToLocalChecked());
		
		Nan::Set(props,  key, value);
	}

	Local<Value> argv[] = { Nan::Null(), props};
	callback->Call(2, argv, async_resource);
}
