#include "SmartHandle.h"

SmartHandle::SmartHandle(HANDLE handle) {
	this->handle = handle;
}

SmartHandle::~SmartHandle() {
	if (this != nullptr && this->handle != NULL) {
		CloseHandle(this->handle);
	}
}
HANDLE SmartHandle::Get() const {
	return this->handle;
}
