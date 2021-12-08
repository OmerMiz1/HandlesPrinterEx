#include "ProcessManager.h"
#include "HandlesManager.h"
#include "utils.h"
#include "consts.h"
#include "types.h"
#include "errors.h"
#include <psapi.h>
#include <winternl.h>

std::vector<DWORD> ProcessManager::ScanProcesses() {
	DWORD aProcesses[1024] = { 0 }, cbNeeded = 0, cProcesses = 0;

	// Get the list of process identifiers.
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		trace_debug(L"ProcessManager::UpdateProcessList: Error EnumProcesses");
		throw MyErrors::ERROR_ENUM_PROCESSES;
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	auto result = std::vector<DWORD>(aProcesses, &(aProcesses[cProcesses]));
	return std::move(result);
	/*this->processes.clear();
	for (unsigned int i = 0; i < cProcesses; i++) {
		this->processes.push_back(aProcesses[i]);
	}*/
}

std::vector<DWORD> ProcessManager::GetPidListByName(std::string procName) {
	auto procHandles = ScanProcesses();

	auto result = std::vector<DWORD>();
	std::string curProcName = "\0";

	for (const DWORD& pid : procHandles) {
		try {
			curProcName = ProcessManager::GetName(pid);
		}
		catch (...) { //TODO make more specific
			continue;
		}

		if (procName.compare(curProcName) == 0) {
			result.push_back(pid);
		}
	}

	return std::move(result);
}

std::string ProcessManager::GetProcessCommandLine(DWORD pid) {
	MY_PUBLIC_PROCESS_BASIC_INFORMATION procInfo{};
	ULONG guessedSize = sizeof(procInfo);
	ULONG requiredSize = 0;  //TODO remove?

	auto procHandle = ProcessManager::Open(pid, PROC_ALL_ACCESS_RIGHTS);
	auto queryProcInfoFunc = (_NtQueryInformationProcess) LoadNtFunction(NT_QUERY_PROC_INFO_NAME);
	auto status = queryProcInfoFunc(procHandle.Get(),
		ProcessBasicInformation,
		&procInfo,
		guessedSize,
		&requiredSize);  //TODO NULL?
	if (status != STATUS_SUCCESS) {
		trace_debug(L"NtQueryInformationProcess failed with status: " + std::to_wstring(status));
		throw MyErrors::ERROR_QUERY_PROC_INFO;
	}
	
	// taken from:
	// https://stackoverflow.com/questions/6530565/getting-another-process-command-line-in-windows/13408150#13408150
	auto pebRemote = procInfo.PebBaseAddress;
	PEB peb{};
	guessedSize = sizeof(PEB);
	MyReadProcessMemory(procHandle.Get(), pebRemote, &peb, guessedSize,	NULL);

	auto paramsRemote = peb.ProcessParameters;
	RTL_USER_PROCESS_PARAMETERS params{};
	guessedSize = sizeof(RTL_USER_PROCESS_PARAMETERS);
	MyReadProcessMemory(procHandle.Get(), paramsRemote,	&params, guessedSize, NULL);

	auto memManager = MemoryManager();
	auto cmdLineRemote = params.CommandLine.Buffer;
	auto cmdLine = (PWSTR) memManager.Alloc(guessedSize);
	guessedSize = params.CommandLine.Length;
	MyReadProcessMemory(procHandle.Get(), cmdLineRemote, cmdLine, guessedSize, NULL);

	return PwstrToStr(cmdLine, guessedSize);
}

SmartHandle ProcessManager::Open(DWORD pid) {
	return ProcessManager::Open(pid, PROC_ACCESS_RIGHTS);
}

SmartHandle ProcessManager::Open(DWORD pid, int accessFlags) {
	auto handle = OpenProcess(accessFlags, FALSE, pid);

	if (handle == INVALID_HANDLE_VALUE) {
		trace_debug(L"OpenProcess has failed");
		throw MyErrors::ERROR_OPEN_PROCESS;
	}

	return SmartHandle(handle);
}

std::string ProcessManager::GetName(DWORD pid) {
	// Taken from: 
	// https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes
	TCHAR nameBuffer[MAX_PATH] = L"\0";
	HMODULE hMod;
	DWORD cbNeeded;

	auto procHandle = ProcessManager::Open(pid);

	// Get the process name
	if (EnumProcessModules(procHandle.Get(), &hMod, sizeof(hMod), &cbNeeded)) {
		GetModuleBaseName(procHandle.Get(), hMod, nameBuffer, MAX_PATH);
	}
	else {
		trace_debug(L"EnumProcessModules failed");
		throw MyErrors::ERROR_ENUM_PROC_MODULES;
	}

	return TcharToStr(nameBuffer);
}


