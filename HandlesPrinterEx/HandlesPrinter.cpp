#include "HandlesPrinter.h"
#include "ProcessManager.h"
#include "SmartHandle.h"
#include "types.h"
#include "consts.h"
#include "utils.h"
#include <iostream>
#include <sstream>

HandlesPrinter::HandlesPrinter() {
	this->handleMan = new HandlesManager();
	this->allowedTypes = std::set<std::string>({ 
		"Key",
		"Section",
		"File",
		"Directory",
		"Thread",
		"Mutant",
		"Semaphore",
		"Desktop",
		"WindowStation" 
	});
	this->multiProcMode = false;
}

HandlesPrinter::~HandlesPrinter() {
	if (this->handleMan != nullptr) {
		delete this->handleMan;
	}
}

void HandlesPrinter::PrintProcHandles(DWORD pid) {
	PrintHeader(pid, this->multiProcMode);

	// Print each proc's handles
	auto procSysHandles = this->handleMan->GetProcessHandles(pid);
	for (const auto& sysHandle : procSysHandles) {
		this->PrintHandle(sysHandle);
	}
}

void HandlesPrinter::PrintProcHandles(std::string processName) {
	auto pidList = ProcessManager::GetPidListByName(processName);
	this->multiProcMode = (pidList.size() > 1); 

	for (auto pid : pidList) {
		this->PrintProcHandles(pid);
	}
}

void HandlesPrinter::PrintProcHandles(int argCount, char** args) {
	ArgumentType argType = ParseArgType(argCount, args);

	switch (argType) {
		case ArgumentType::PID: {
			DWORD pid = std::stoul(args[1]);
			this->PrintProcHandles(pid);
			break;
		}
		case ArgumentType::ProcessName: {
			std::string procName = std::string(args[1]);
			this->PrintProcHandles(procName);
			break;
		}
		case ArgumentType::InvalidArgumentCount: {
			std::cout << "Invalid arguments count, 1 argument expected!" << std::endl;
			break;
		}
		case ArgumentType::InvalidProcessName: {
			std::cout << "Invalid argument name, process name can't contain the follwing chars: " << PROC_NAME_BAD_CHARS << std::endl;
			break;
		}
		default: {
			std::cout << "Unknown argument error" << std::endl;
			break;
		}
	}
}


// Class Utils (pricate funcs) //
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

void HandlesPrinter::PrintHandle(SYSTEM_HANDLE sysHandle) const {
	auto name = this->handleMan->GetHandleName(sysHandle);
	auto type = this->handleMan->GetHandleType(sysHandle);
	auto ptrCount = this->handleMan->GetHandlePointerCount(sysHandle);
	auto handleCount = this->handleMan->GetHandleCount(sysHandle);
	
	// Case handle type not allowed, not printing it
	if (!IsAllowedType(type)) {
		type = UNKNOWN_HANDLE_TYPE;
	}

	std::ostringstream stream;
	stream << " " << std::hex << sysHandle.Handle;
	stream << ": " << type;
	stream << " " << name;
	
	if (ptrCount > INVALID_HANDLE_PTR_COUNT) {
		stream << "\tpointers count: " << std::dec << ptrCount;
	} else {
		stream << "\t" << UNKNOWN_HANDLE_POINTER_COUNT_MSG;
	}
	
	if (handleCount > INVALID_HANDLES_COUNT) {
		stream << "\thandles count: " << handleCount;
	} else {
		stream << "\t" << UNKNOWN_HANDLES_COUNT_MSG;
	}

	std::cout << stream.str() << std::endl;
}

bool HandlesPrinter::IsAllowedType(std::string type) const {
	return (this->allowedTypes.count(type.c_str()) > 0);
}

