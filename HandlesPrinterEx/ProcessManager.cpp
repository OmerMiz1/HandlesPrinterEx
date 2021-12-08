#include "MyException.h"
#include "ProcessManager.h"
#include "HandlesManager.h"
#include "utils.h"
#include "consts.h"
#include "types.h"
#include "errors.h"
#include <psapi.h>
#include <winternl.h>

std::vector<DWORD> ProcessManager::ScanProcesses()
{
	// Taken from MSDN
	DWORD aProcesses[1024] = { 0 }, cbNeeded = 0, cProcesses = 0;

	// Get the list of process identifiers.
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		throw_exception(ERROR_ENUM_PROCESSES);
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	auto result = std::vector<DWORD>(aProcesses, &(aProcesses[cProcesses]));
	return std::move(result);
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

	auto procHandle = ProcessManager::Open(pid, PROCESS_ALL_ACCESS);
	auto queryProcInfoFunc = (_NtQueryInformationProcess) LoadNtFunction(NT_QUERY_PROC_INFO_NAME);
	auto status = queryProcInfoFunc(procHandle.Get(),
		ProcessBasicInformation,
		&procInfo,
		guessedSize,
		&requiredSize);  //TODO NULL?
	if (status != STATUS_SUCCESS) {
		SetLastError(status);
		throw_exception_with_status(ERROR_QUERY_PROC_INFO, status);
	}
	
	// taken from:
	// https://stackoverflow.com/questions/6530565/getting-another-process-command-line-in-windows/13408150#13408150
	auto pebRemote = procInfo.PebBaseAddress;
	PEB peb{};
	guessedSize = sizeof(PEB);
	status = ReadProcessMemory(procHandle.Get(), pebRemote, &peb, guessedSize, NULL);
	if (status == 0) {
		throw_exception(ERROR_READ_PROC_MEM);
	}

	auto paramsRemote = peb.ProcessParameters;
	RTL_USER_PROCESS_PARAMETERS params{};
	guessedSize = sizeof(RTL_USER_PROCESS_PARAMETERS);
	status = ReadProcessMemory(procHandle.Get(), paramsRemote, &params, guessedSize, NULL);
	if (status == 0) {
		throw_exception(ERROR_READ_PROC_MEM);
	}

	auto memManager = MemoryManager();
	auto cmdLineRemote = params.CommandLine.Buffer;
	auto cmdLine = (PWSTR) memManager.Alloc(guessedSize);
	guessedSize = params.CommandLine.Length;
	status = ReadProcessMemory(procHandle.Get(), cmdLineRemote, cmdLine, guessedSize, NULL);
	if (status == 0) {
		throw_exception(ERROR_READ_PROC_MEM);
	}

	return ToString(cmdLine, guessedSize);
}

SmartHandle ProcessManager::Open(DWORD pid) {
	return ProcessManager::Open(pid, PROC_ACCESS_RIGHTS);
}

SmartHandle ProcessManager::Open(DWORD pid, int accessFlags) {
	auto handle = OpenProcess(accessFlags, FALSE, pid);

	if (handle == INVALID_HANDLE_VALUE) {
		throw_exception(ERROR_OPEN_PROCESS);
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
	} else {
		throw_exception(ERROR_ENUM_PROC_MODULES);
	}

	return ToString(nameBuffer);
}


