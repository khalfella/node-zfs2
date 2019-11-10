#include <nan.h>
#include <iostream>
#include <libzfs.h>
#include <map>

using namespace v8;
using namespace Nan;

class ZFSGetWorker : public AsyncWorker {
	public:
		ZFSGetWorker(Nan::Callback *callback, std::string name);
		void Execute();
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		zfs_handle_t *zfsh;
		std::map<std::string, std::string> string_props;
		std::map<std::string, double> numeric_props;
};
