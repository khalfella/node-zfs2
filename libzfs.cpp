#include <nan.h>
#include <iostream>

using namespace Nan;
using namespace v8;

class ZFSGetWorker: public AsyncWorker {
	public:
		ZFSGetWorker(Nan::Callback *callback, std::string name) :
		AsyncWorker(callback) {
			this->name = name;
		}


		void Execute() {
			// do nothing for now
		}

		void HandleOKCallback() {
			Local<Value> argv[] = {
				Nan::Null(),
				Nan::New<String>(this->name.c_str()).ToLocalChecked()
			};
			callback->Call(2, argv, async_resource);
		}
	private:
		std::string name;
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
