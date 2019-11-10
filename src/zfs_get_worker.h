#ifndef __ZFS_GET_WORKER__
#define __ZFS_GET_WORKER__


#include <nan.h>
#include <map>

using namespace v8;
using namespace Nan;

class __zfs_prop__ {
	public:
		std::string	name;
		bool		is_num;
		std::string	value;
		std::string	source;
		std::string	where;
};

class ZFSGetWorker : public AsyncWorker {
	public:
		ZFSGetWorker(Nan::Callback *callback, std::string name);
		void Execute();
		void HandleOKCallback();
	private:
		std::string name;
		std::string errorMessage;
		zfs_handle_t *zfsh;
		std::vector<__zfs_prop__> props;
};

#endif // __ZFS_GET_WORKER__
