#include "HandlesPrinter.h"
#include "ProcessManager.h"
#include "SmartHandle.h"
#include "types.h"
#include "consts.h"
#include <iostream>
#include <vector>
#include <sstream>

HandlesPrinter::HandlesPrinter() {
	this->handleMan = new HandlesManager();
	this->allowedTypes = std::set<std::string>({ "Key",
		"Section",
		"File",
		"Directory",
		"Thread",
		"Mutant",
		"Semaphore",
		"Desktop",
		"WindowStation" 
	});
}

HandlesPrinter::~HandlesPrinter() {
	if (this->handleMan != nullptr) {
		delete this->handleMan;
	}
}

void HandlesPrinter::PrintProcHandles(DWORD pid, bool multiProcMode) {
	PrintHeader(pid, multiProcMode);

	// Print each proc's handles
	auto procSysHandles = this->handleMan->GetProcessHandles(pid);
	for (const auto& sysHandle : procSysHandles) {
		this->PrintHandle(sysHandle);
	}
}

void HandlesPrinter::PrintProcHandles(std::string processName) {
	auto pidList = ProcessManager::GetPidListByName(processName);
	auto multiProcMode = (pidList.size() > 1); 

	for (auto pid : pidList) {
		this->PrintProcHandles(pid, multiProcMode);
	}
}

void HandlesPrinter::PrintHeader(DWORD pid, bool multiProcMode) {
	std::ostringstream stream;

	stream << "-------------------------------------------------" << std::endl;
	auto procName = ProcessManager::GetName(pid);
	stream << procName.c_str() << "\tpid: " << pid;

	if (multiProcMode) {
		auto procCmd = ProcessManager::GetProcessCommandLine(pid);
		stream << "\tcommand line: " << procCmd.c_str();
	}

	std::cout << stream.str() << std::endl;
}

void HandlesPrinter::PrintHandle(SYSTEM_HANDLE sysHandle) {
	auto name = this->handleMan->GetHandleName(sysHandle);
	auto type = this->handleMan->GetHandleType(sysHandle);
	auto ptrCount = this->handleMan->GetHandlePointerCount(sysHandle);
	auto handleCount = this->handleMan->GetHandleCount(sysHandle);
	
	// Case handle type not allowed, print unknown type
	if (!IsAllowedType(type)) {
		type = UNKNOWN_HANDLE_TYPE;
	}

	std::ostringstream stream;
	stream << " " << std::hex << sysHandle.Handle;
	stream << ": " << type;
	stream << " " << name;
	
	if (ptrCount > 0) {
		stream << "\tpointers count: " << std::dec << ptrCount;
	} else {
		stream << "\t" << UNKNOWN_HANDLE_POINTER_COUNT;
	}
	
	if (handleCount > 0) {
		stream << "\thandles count: " << handleCount;
	} else {
		stream << "\t" << UNKNOWN_HANDLE_COUNT;
	}

	std::cout << stream.str() << std::endl;
}

bool HandlesPrinter::IsAllowedType(std::string type) {
	return (this->allowedTypes.count(type.c_str()) > 0);
}

