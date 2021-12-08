#include "MemoryManager.h"
#include "utils.h"
#include "consts.h"
#include "errors.h"
#include <iostream>

MemoryManager::~MemoryManager() {
	for (auto addr : this->virtualAddrs) {
		try {
			this->Free(addr);
		} catch (MyException e) {
			// Do nothing, let the debug message print (from Free)
			// and try to free the rest of the memory before ending.
			// Consider enabling a flag to alert that free has failed, 
			// maybe retry in another function or something.
			e.what(); // returns msg, but also prints to debug
		}
	}
}

LPVOID MemoryManager::Alloc(SIZE_T bytesToAllocate) {
	auto resultAddr = VirtualAlloc(NULL, bytesToAllocate, MEM_COMMIT, PAGE_READWRITE);

	if (resultAddr == nullptr) {
		throw_exception(ERROR_VIRTUAL_ALLOC);
	}

	// Store addr and size for later free
	this->virtualAddrs.push_back(resultAddr);
	this->sizes[resultAddr] = bytesToAllocate;

	return resultAddr;
}

LPVOID MemoryManager::Realloc(LPVOID addr, SIZE_T bytesToAllocate) {
	this->Free(addr);
	return this->Alloc(bytesToAllocate);
}

void MemoryManager::Free(LPVOID addr) {
	if (addr == nullptr) {
		throw_exception(ERROR_VIRTUAL_FREE_NULLPTR);
	}

	if (!VirtualFree(addr, this->sizes[addr], MEM_DECOMMIT)) {
		throw_exception(ERROR_VIRTUAL_FREE);
	}

	// Remove from addrs to free
	auto toRemove = std::find(this->virtualAddrs.begin(), this->virtualAddrs.end(), addr);
	if (toRemove != this->virtualAddrs.end()) {
		this->virtualAddrs.erase(toRemove);
		this->sizes.erase(addr);
	}
}