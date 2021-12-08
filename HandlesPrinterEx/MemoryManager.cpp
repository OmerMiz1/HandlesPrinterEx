#include "MemoryManager.h"
#include "utils.h"
#include "consts.h"
#include "errors.h"
#include <iostream>

MemoryManager::~MemoryManager() {
	for (auto addr : this->virtualAddrs) {
		try {
			this->Free(addr);
		} catch (...) {
			// Do nothing, let the debug message print (from Free)
			// and try to free the rest of the memory before ending.
			// Consider enabling a flag to alert that free has failed, 
			// maybe retry in another function or something.
		}
	}
}

LPVOID MemoryManager::Alloc(SIZE_T bytesToAllocate) {
	auto resultAddr = VirtualAlloc(NULL, bytesToAllocate, MEM_COMMIT, MEM_ALLOC_PROTECTION);

	if (resultAddr == nullptr) {
		trace_debug(L"VirtualAlloc failed");
		throw MyErrors::ERROR_VIRTUAL_ALLOC;
	}

	this->virtualAddrs.push_back(resultAddr);
	this->sizes[resultAddr] = bytesToAllocate; //TODO remove
	return resultAddr;
}

LPVOID MemoryManager::Realloc(LPVOID addr, SIZE_T bytesToAllocate) {
	if (addr != nullptr) { //TODO handle error else?
		this->Free(addr);
	}

	return this->Alloc(bytesToAllocate);
}

void MemoryManager::Free(LPVOID addr) {
	if (addr == nullptr) {
		trace_debug(L"cant free null address");
		return;
	} else if (!VirtualFree(addr, this->sizes[addr], MEM_DECOMMIT)) {
		trace_debug(L"VirtualFree failed");
		throw MyErrors::ERROR_VIRTUAL_FREE;
	}

	auto toRemove = std::find(this->virtualAddrs.begin(), this->virtualAddrs.end(), addr);
	if (toRemove != this->virtualAddrs.end()) {
		this->virtualAddrs.erase(toRemove);
	}
	this->sizes.erase(addr);
}