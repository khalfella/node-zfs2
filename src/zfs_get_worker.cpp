#include <nan.h>

#include "zfs_get_worker.h"

using namespace Nan;
using namespace v8;

ZFSGetWorker::ZFSGetWorker(Nan::Callback *callback, std::string name)
    : ZFSWorker(callback) {
	this->name = name;
	this->errorMessage = "";
}


void ZFSGetWorker::Run(ZFSWorkerLibZFSHandle *lzfsh) {

	if (lzfsh == NULL) {
		this->errorMessage = "Error initializing libzfs";
		return;
	}
	if ((this->zfsh = ZFSWorker::ZFSOpen(lzfsh, this->name.c_str(),
	    ZFSWorkerZFSTypeDataset)) == NULL) {
		this->errorMessage = "Failed to open dataset: " + this->name;
		return;
	}

	auto cb = [](int pr, void *data)  {
		auto self = (ZFSGetWorker*) data;
		auto prop = (ZFSWorkerZFSProp) pr;

		ZFSWorkerZPropSource source;
		char value_buf[ZFSWorkerZFSMaxPropLen];
		char where_buf[ZFSWorkerZFSMaxPropLen];
		if (ZFSWorker::ZFSPropGet(self->zfsh, prop, value_buf, sizeof(value_buf),
		    &source, where_buf, sizeof(where_buf), true) == 0) {

			ZFSProperty p;
			p.prop = pr;
			p.name = ZFSWorker::ZFSPropToName(prop);
			p.value = value_buf;
			p.source = source;
			p.where = where_buf;

			self->props.push_back(p);
		}
		return ((int)ZFSWorkerZFSPropCont);
	};

	ZFSWorker::ZFSPropIter(cb, this, true, true, (ZFSWorkerZFSType) ZFSWorkerZFSTypeDataset);
	ZFSWorker::ZFSClose(zfsh);
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

	for (auto prop = this->props.begin(); prop != this->props.end(); prop++) {
		Local<String> key = Nan::New<String>(prop->name).ToLocalChecked();
		Local<Object> value = Nan::New<Object>();
		double val;
		if (prop->MayBeNumericZFSProperty(&val)) {
			Nan::Set(value,
			    Nan::New<String>("value").ToLocalChecked(),
			    Nan::New<Number>(val));
		} else {
			Nan::Set(value,
			    Nan::New<String>("value").ToLocalChecked(),
			    Nan::New<String>(prop->value.c_str()).ToLocalChecked());
		}

		Nan::Set(value,
		    Nan::New<String>("source").ToLocalChecked(),
		    Nan::New<String>(prop->SourceToString().c_str()).ToLocalChecked());
		Nan::Set(value,
		    Nan::New<String>("where").ToLocalChecked(),
		    Nan::New<String>(prop->WhereToString().c_str()).ToLocalChecked());

		Nan::Set(props,  key, value);
	}

	Local<Value> argv[] = { Nan::Null(), props};
	callback->Call(2, argv, async_resource);
}
