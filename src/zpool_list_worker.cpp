#include <nan.h>

#include "zpool_list_worker.h"

using namespace Nan;
using namespace v8;

ZPoolListWorker::ZPoolListWorker(Nan::Callback *callback) : ZFSWorker(callback) {
	this->errorMessage = "";
}

void ZPoolListWorker::Run(libzfs_handle_t *lzfsh) {

	if (lzfsh == NULL) {
		this->errorMessage = "error initializing libzfs";
		return;
	}

	auto zpool_iter_cb = [](zpool_handle_t *zph, void *data) {

		auto zpool_prop_iter_cb = [](int p, void *data) {
			auto prop = (zpool_prop_t) p;
			auto *self = (ZPoolListWorker*) data;

			zprop_source_t source;
			char value_buf[ZFS_MAXPROPLEN];
			if (zpool_get_prop(self->cur_zph, prop, value_buf, sizeof (value_buf),
			    &source, _B_TRUE) == 0) {
				ZFSProperty p;
				p.name = zpool_prop_to_name(prop);
				p.value = value_buf;

				// XXX: Make use of these
				p.is_num = false;
				p.source = source;
				p.where = "";

				self->cur_zprops->push_back(p);
			}
			return ((int)ZPROP_CONT);
		};

		auto *self = (ZPoolListWorker*) data;
		std::vector<ZFSProperty> zprops;
		self->cur_zph = zph;
		self->cur_zprops = &zprops;
		zprop_iter(zpool_prop_iter_cb, self, _B_TRUE, _B_TRUE, (zfs_type_t) ZFS_TYPE_POOL);
		zpool_close(zph);

		self->props.push_back(zprops);
		return ((int)ZPROP_CONT);
	};

	zpool_iter(lzfsh, zpool_iter_cb, this);
}

void ZPoolListWorker::HandleOKCallback() {
	if (this->errorMessage.length() > 0) {
		Local<Value> argv[] = {
		    Nan::New<String>(this->errorMessage.c_str()).ToLocalChecked()
		};
		callback->Call(1, argv, async_resource);
		return;
	}

	Local<Array> zps = New<v8::Array>(this->props.size());
	int index = 0;
	for (auto zp = this->props.begin(); zp != this->props.end(); zp++) {
		Local<Object> props = Nan::New<Object>();
		for (auto prop = zp->begin(); prop != zp->end(); prop++) {
			Local<String> key = Nan::New<String>(prop->name).ToLocalChecked();
			Local<Object> value = Nan::New<Object>();

			Nan::Set(value,
			    Nan::New<String>("value").ToLocalChecked(),
			    Nan::New<String>(prop->value.c_str()).ToLocalChecked());

			Nan::Set(props,  key, value);
		}

		Nan::Set(zps, index++, props);
	}

	Local<Value> argv[] = { Nan::Null(), zps};
	callback->Call(2, argv, async_resource);
}
