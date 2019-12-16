#include "zfs_worker.h"

#include <stdlib.h>

using namespace Nan;
using namespace v8;

std::string ZFSProperty::SourceToString() {
	switch(this->source) {
		case ZPROP_SRC_NONE:
			return "-";
		break;
		case ZPROP_SRC_DEFAULT:
			return "default";
		break;
		case ZPROP_SRC_TEMPORARY:
			return "temporary";
		break;
		case ZPROP_SRC_LOCAL:
			return "local";
		break;
		case ZPROP_SRC_INHERITED:
			return "inherited";
		break;
		case ZPROP_SRC_RECEIVED:
			return "received";
		break;
		default:
			return "unknown";
		break;
	}
}

std::string ZFSProperty::WhereToString() {
	if (this->source == ZPROP_SRC_INHERITED) {
		return this->where;
	}
	return "";
}

bool ZFSProperty::MayBeNumericZFSProperty(double *num) {
	if (!zfs_prop_is_string((zfs_prop_t)this->prop)) {
		char *endptr;
		*num = strtod(this->value.c_str(),  &endptr);
		return (*endptr == 0);
	}
	return false;
}
/*
 * As the time of this writing, there is no reliable way to determine if a zpool
 * property is a numberic property or a string property. Conterary to the case
 * of zfs properties, where were can use zfs_prop_is_string() there is no such
 * function for zpool properties. We need to guess if a property is a numeric
 * property because we want to retrun a javascript Number whenever possible.
 * This function uses the knowledge about properties we know about in order
 * to guess if the property is a numeric or not.
 *
 * Note: A a result of being conservative here, any new numeric zpool property
 * added after writing this code will show up as string. I would love to know
 * if there is a better way to handle this situation.
 */
bool ZFSProperty::MayBeNumericZpoolProperty(double *num) {
	const char *numericProps[] = {
	    "size", "free", "freeing", "leaked", "allocated", "capacity",
	    "guid", "ashift", "maxblocksize", "maxdnodesize"
	};

	std::vector<std::string> props(numericProps, std::end(numericProps));
	for (auto prop = props.begin(); prop != props.end(); prop++) {
		if (this->name == *prop) {
			char *endptr;
			*num = strtod(this->value.c_str(),  &endptr);
			return (*endptr == 0);
		}
	}
	return false;
}

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
