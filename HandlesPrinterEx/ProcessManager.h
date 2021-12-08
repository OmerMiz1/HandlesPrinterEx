#pragma once

#include "MemoryManager.h"
#include "SmartHandle.h"
#include <vector>
#include <string>
#include <windows.h>

class ProcessManager
{
private:
	ProcessManager(); // Static class
	static std::vector<DWORD> ScanProcesses();

public:
	static std::vector<DWORD> GetPidListByName(std::string name) throw(...); //TODO throw
	static std::string GetProcessCommandLine(DWORD pid) throw(...); //TOOD throw
	static SmartHandle Open(DWORD pid);
	static SmartHandle Open(DWORD pid, int accessFlags);
	static std::string GetName(DWORD pid);
};

