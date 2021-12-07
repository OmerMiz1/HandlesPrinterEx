#include "HandlesPrinter.h"
#include "ProcessManager.h"
#include "SmartHandle.h"
#include "types.h"
#include <iostream>
#include <vector>
#include <sstream>

void HandlesPrinter::Print(DWORD pid, bool multiProcMode) {
	auto procSysHandles = this->handleMan.GetProcessHandles(pid);
	PrintHeader(pid, multiProcMode);

	for (auto sysHandle : procSysHandles) {
		auto name = this->handleMan.GetHandleName(sysHandle);
		auto type = this->handleMan.GetHandleType(sysHandle);
		auto ptrCount = this->handleMan.GetPointerCount(sysHandle);
		auto handleCount = this->handleMan.GetHandleCount(sysHandle);
		std::string cmdLine = "";
		

		std::ostringstream stream;
		stream << "  " << std::hex << sysHandle.Handle;
		stream << ": " << type;
		stream << " " << name;
		stream << "\tpointers count: " << std::dec << ptrCount;
		stream << "\thandles count: " << handleCount;

		std::cout << stream.str() << std::endl;
	}
}

void HandlesPrinter::Print(std::string processName) {
	auto procMan = ProcessManager();
	auto pidList = procMan.GetPidListByName(processName);
	auto multiProcMode = (pidList.size() > 1); 

	for (auto pid : pidList) {
		//procMan.GetProcessCommandLine(pid);
		this->Print(pid, multiProcMode);
	}
}

void HandlesPrinter::PrintHeader(DWORD pid, bool multiProcMode) {
	char header[MAX_PATH] = { "\0" };
	auto dashes = "-------------------------------------------------";
	auto format = "%s\tpid:%lu";
	auto procName = Process::GetName(pid);

	//TODO? if multiProcMode

	// TODO change to relevant name (not MAX_PATH)
	sprintf_s(header, MAX_PATH, format, procName.c_str(), pid);
	std::cout << dashes << std::endl << header << std::endl;
}


