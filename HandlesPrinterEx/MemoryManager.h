#pragma once

#include "MyException.h"
#include <vector>
#include <map>
#include <windows.h>

// Class note:
/**
* Made this class to make sure that every dynamic allocation
* will be freed at the end of execution.
*/
class MemoryManager
{
private:
	std::vector<LPVOID> virtualAddrs = std::vector<LPVOID>(); // allocated addresses
	std::map<LPVOID, SIZE_T> sizes = std::map<LPVOID, SIZE_T>(); // maps address to size 

	/**
	* Only called from DTOR.
	*/
	void Free(LPVOID addr) throw(MyException);

public:
	~MemoryManager();

	/**
	* Allocates memory by calling VirtualAlloc with MEM_COMMIT
	* and PAGE_READWRITE access.
	* 
	* May throw MyException (see errors.h and MyException.h)
	*/
	LPVOID Alloc(SIZE_T size) throw(MyException);
	/**
	* A convinience function, reallocates virtual memory
	* by calling VirtualFree and VirtualAlloc.
	*/
	LPVOID Realloc(LPVOID addr, SIZE_T bytesToAllocate) throw(MyException);
};

