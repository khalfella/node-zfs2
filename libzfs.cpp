#include <nan.h>
#include <iostream>
#include <libzfs.h>

using namespace Nan;
using namespace v8;

class ZFSGetWorker: public AsyncWorker {
	public:
		ZFSGetWorker(Nan::Callback *callback, std::string name) :
		AsyncWorker(callback) {
			this->name = name;
			this->props = NULL;
			this->errorMessage = "";
		}


		void Execute() {
			libzfs_handle_t *lh;
			zfs_handle_t *zfsh;

			if ((lh = libzfs_init()) == NULL) {
				this->errorMessage = "error initialized libzfs";
				return;
			}
			if ((zfsh = zfs_open(lh, this->name.c_str(), ZFS_TYPE_DATASET)) == NULL) {
				this->errorMessage = "error initialized libzfs";
				goto out2;
			}

			if((this->props = zfs_get_user_props(zfsh)) == NULL) {
				this->errorMessage = "failed to get dataset properties";
				goto out1;
			}

			//nvlist_free(props);
			// We are leaking handles!!
			return;

out1:
			zfs_close(zfsh);
out2:
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

			nvpair_t *nvp;
			for (nvp = nvlist_next_nvpair(this->props, NULL); nvp != NULL;
			    nvp = nvlist_next_nvpair(this->props, nvp)) {

				Local<String> key = Nan::New<String>(nvpair_name(nvp)).ToLocalChecked();
				data_type_t ptype = nvpair_type(nvp);
				std::cout<<"iteration" << (int) ptype;

				if (ptype == DATA_TYPE_STRING) {
					char *str;
					nvpair_value_string(nvp, &str);
					Nan::Set(props, key, Nan::New<String>(str).ToLocalChecked());
				} else if( ptype == DATA_TYPE_BOOLEAN) {
					boolean_t b;
					nvpair_value_boolean_value(nvp, &b);
					Nan::Set(props, key, Nan::New<Boolean>(b));
				}
			}

			Local<Value> argv[] = { Nan::Null(), props};
			callback->Call(2, argv, async_resource);
		}
	private:
		std::string name;
		std::string errorMessage;
		nvlist_t *props;
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

NODE_MODULE(libzfs, Init)
