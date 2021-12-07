#pragma once

#include "Process.h"
#include "MemoryManager.h"
#include <vector>
#include <string>
#include <windows.h>

class ProcessManager
{
private:
	MemoryManager memManager = MemoryManager();
	std::vector<DWORD> processes = std::vector<DWORD>();
	void ScanProcesses();

public:
	std::vector<DWORD> GetPidListByName(std::string name);  //TODO handle errors
	std::string GetProcessCommandLine(DWORD pid); //TOOD throws
};

