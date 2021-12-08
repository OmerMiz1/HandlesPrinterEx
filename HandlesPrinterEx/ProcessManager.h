#pragma once

#include "MemoryManager.h"
#include "MyException.h"
#include "SmartHandle.h"
#include <vector>
#include <string>
#include <windows.h>

class ProcessManager
{
private:
	ProcessManager(); // Static class
	static std::vector<DWORD> ScanProcesses() throw(MyException);

public:
	static std::vector<DWORD> GetPidListByName(std::string name) throw(MyException);
	static std::string GetProcessCommandLine(DWORD pid) throw(MyException);
	static SmartHandle Open(DWORD pid) throw(MyException);
	static SmartHandle Open(DWORD pid, int accessFlags) throw(MyException);
	static std::string GetName(DWORD pid) throw(MyException);
};

