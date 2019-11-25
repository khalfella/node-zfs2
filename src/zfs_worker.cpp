#include "zfs_worker.h"

using namespace Nan;
using namespace v8;


// Initialize static variables
libzfs_handle_t *ZFSWorker::lzfsh = NULL;
std::mutex ZFSWorker::lzfsLock;

ZFSWorker::ZFSWorker(Nan::Callback *callback) : AsyncWorker(callback) {
}

void ZFSWorker::Execute() {
	std::lock_guard<std::mutex> lck(ZFSWorker::lzfsLock);
	this->Run(ZFSWorker::lzfsh);
}

void ZFSWorker::Run(libzfs_handle_t *lzfsh) {
}

void ZFSWorker::InitializeLibZFS() {
	std::lock_guard<std::mutex> lck(ZFSWorker::lzfsLock);
	ZFSWorker::lzfsh = libzfs_init();
}
