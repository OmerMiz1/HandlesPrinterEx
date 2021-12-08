#pragma once

#include "HandlesManager.h"
#include <string>
#include <set>

class HandlesPrinter
{
private:
	HandlesManager* handleMan;  //TODO handle error (CTOR)
	std::set<std::string> allowedTypes;
	static void PrintHeader(DWORD pid, bool multiProcMode);
	void PrintHandle(SYSTEM_HANDLE sysHandle);
	bool IsAllowedType(std::string type);

public:
	HandlesPrinter();
	~HandlesPrinter();
	void PrintProcHandles(DWORD pid, bool multiProcMode);
	void PrintProcHandles(std::string processName);
};
