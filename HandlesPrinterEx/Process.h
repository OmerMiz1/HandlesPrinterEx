#pragma once

#include "SmartHandle.h"
#include <string>
#include <windows.h>

class Process
{
private:
	Process();  // Static class

public:
	static SmartHandle Open(DWORD pid);
	static SmartHandle Open(DWORD pid, int accessFlags);
	static std::string GetName(DWORD pid);
	static SmartHandle Duplicate(const SmartHandle& src);
};

