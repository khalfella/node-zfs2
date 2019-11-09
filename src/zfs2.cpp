#include <nan.h>
#include <iostream>
#include <libzfs.h>
#include <map>

using namespace Nan;
using namespace v8;

class ZFSGetWorker: public AsyncWorker {
	public:
		ZFSGetWorker(Nan::Callback *callback, std::string name) :
		AsyncWorker(callback) {
			this->name = name;
			this->errorMessage = "";
		}


		void Execute() {
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

			auto cb = [](int prop, void *data)  {
				ZFSGetWorker *self = (ZFSGetWorker*) data;

				char buf[ZFS_MAXPROPLEN];
				if (zfs_prop_get(self->zfsh, (zfs_prop_t) prop, buf, sizeof(buf), NULL, NULL, 0, _B_TRUE) == 0) {
					if (zfs_prop_is_string((zfs_prop_t) prop)) {
						self->string_props.insert(std::pair<std::string, std::string>(zfs_prop_to_name((zfs_prop_t) prop), buf));
					} else {
						self->numeric_props.insert(std::pair<std::string, double>(zfs_prop_to_name((zfs_prop_t) prop), std::stod(buf)));
					}
				}

				return ((int)ZPROP_CONT);
			};

			zprop_iter(cb, this, _B_TRUE, _B_TRUE, (zfs_type_t) ZFS_TYPE_DATASET);

			zfs_close(zfsh);
			libzfs_fini(lh);
		}

		void HandleOKCallback() {

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
	private:
		std::string name;
		std::string errorMessage;
		zfs_handle_t *zfsh;
		std::map<std::string, std::string> string_props;
		std::map<std::string, double> numeric_props;
};

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

NAN_MODULE_INIT(Init) {
	Nan::Set(target,
	    New<String>("zfsGet").ToLocalChecked(),
	    GetFunction(New<FunctionTemplate>(zfsGet))
	        .ToLocalChecked());
}

NODE_MODULE(zfs2, Init)
