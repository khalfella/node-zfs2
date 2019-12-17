#ifndef __ZFS_WORKER__
#define __ZFS_WORKER__

#include <nan.h>
#include <mutex>
#include <libzfs.h>

typedef libzfs_handle_t	ZFSWorkerLibZFSHandle;
typedef	zpool_handle_t	ZFSWorkerZPoolHandle;
typedef zfs_handle_t	ZFSWorkerZFSHandle;
typedef	zpool_prop_t	ZFSWorkerZPoolProp;
typedef zfs_prop_t	ZFSWorkerZFSProp;
typedef zprop_source_t	ZFSWorkerZPropSource;
typedef zfs_type_t	ZFSWorkerZFSType;
typedef zprop_func	ZFSWorkerZFSPropFunc;


#define	ZFSWorkerZPoolPropHealth	ZPOOL_PROP_HEALTH
#define ZFSWorkerZFSTypeDataset		ZFS_TYPE_DATASET
#define ZFSWorkerZFSMaxPropLen		ZFS_MAXPROPLEN

#define	ZFSWorkerZFSPropCont		ZPROP_CONT

using namespace v8;
using namespace Nan;

class ZFSProperty {
	public:
		bool MayBeNumericZFSProperty(double *);
		bool MayBeNumericZpoolProperty(double *);
		std::string SourceToString();
		std::string WhereToString();
	public:
		int			prop;
		std::string		name;
		std::string		value;
		ZFSWorkerZPropSource	source;
		std::string		where;
};

class ZFSWorker : public AsyncWorker {
	public:
		ZFSWorker(Nan::Callback *);
		void Execute();
		virtual void Run(ZFSWorkerLibZFSHandle *);
		static void InitializeLibZFS();
		static ZFSWorkerZFSHandle *ZFSOpen(ZFSWorkerLibZFSHandle *, const char *, int);
		static void ZFSClose(ZFSWorkerZFSHandle *);
		static int ZFSPropGet(ZFSWorkerZFSHandle *, ZFSWorkerZFSProp, char *, size_t,
		    ZFSWorkerZPropSource *, char *, size_t, bool);
		static const char *ZFSPropToName(ZFSWorkerZFSProp);
		static int ZFSPropIter(ZFSWorkerZFSPropFunc, void *, bool, bool, ZFSWorkerZFSType);
		static ZFSWorkerZPoolHandle *ZPoolOpen(ZFSWorkerLibZFSHandle*, const char*);
		static void ZPoolClose(ZFSWorkerZPoolHandle *);
		static int ZPoolGetProp(ZFSWorkerZPoolHandle *, ZFSWorkerZPoolProp, char *, size_t,
		    ZFSWorkerZPropSource *, bool);
	private:
                static ZFSWorkerLibZFSHandle *lzfsh;
		static std::mutex lzfsLock;
};


#endif // __ZFS_WORKER__
