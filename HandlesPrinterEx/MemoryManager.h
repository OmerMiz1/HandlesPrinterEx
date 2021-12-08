#pragma once

#include <vector>
#include <map>
#include <windows.h>

class MemoryManager
{
private:
	std::vector<LPVOID> virtualAddrs = std::vector<LPVOID>();
	std::map<LPVOID, SIZE_T> sizes = std::map<LPVOID, SIZE_T>();
	void Free(LPVOID addr) throw(...);  //TODO throws

public:
	~MemoryManager();

	LPVOID Alloc(SIZE_T size) throw(...);  //TODO throws
	LPVOID Realloc(LPVOID addr, SIZE_T bytesToAllocate) throw(...);
};

