#include "MemoryManager.h"
#include "utils.h"
#include "consts.h"

MemoryManager::~MemoryManager() {
	for (auto addr : this->virtualAddrs) {
		this->Free(addr);
	}
}

LPVOID MemoryManager::Alloc(SIZE_T bytesToAllocate) {
	auto resultAddr = VirtualAlloc(NULL, bytesToAllocate, MEM_ALLOC_TYPE, MEM_ALLOC_PROTECTION);
	
	if (resultAddr == nullptr) {
		trace_debug(L"VirtualAlloc failed");
		//TODO? handle error
	}

	this->virtualAddrs.push_back(resultAddr);
	return resultAddr;
}

LPVOID MemoryManager::Realloc(LPVOID addr, SIZE_T bytesToAllocate) {
	this->Free(addr);
	return this->Alloc(bytesToAllocate);
}

int MemoryManager::Free(LPVOID addr) {
	if (addr == nullptr) {
		trace_debug(L"address to free cant be null");
		//TODO? handle error
		return 0;
	} else if (!VirtualFree(addr, 0, MEM_RELEASE)) {
		trace_debug(L"VirtualFree failed");
		//TODO? handle error
		return 0;
	}

	auto toRemove = std::find(this->virtualAddrs.begin(), this->virtualAddrs.end(), addr);
	this->virtualAddrs.erase(toRemove);
	return 1;
}