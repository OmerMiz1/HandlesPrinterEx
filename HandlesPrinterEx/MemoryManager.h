#pragma once

#include <vector>
#include <windows.h>

class MemoryManager
{
private:
	std::vector<LPVOID> virtualAddrs = std::vector<LPVOID>();

public:
	~MemoryManager();

	LPVOID Alloc(SIZE_T size);
	LPVOID Realloc(LPVOID addr, SIZE_T bytesToAllocate);
	int Free(LPVOID addr);
};

