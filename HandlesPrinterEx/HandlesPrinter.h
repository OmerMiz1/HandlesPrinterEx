#pragma once

#include "HandlesManager.h"
#include <set>
#include <string>

class HandlesPrinter
{
private:
	HandlesManager handleMan = HandlesManager();
	static void PrintHeader(DWORD pid, bool multiProcMode);

public:
	void Print(DWORD pid, bool multiProcMode);
	void Print(std::string processName);
};
