#include "ProcessManager.h"
#include "HandlesManager.h"
#include "utils.h"
#include "consts.h"
#include "types.h"
#include <psapi.h>
#include <winternl.h>

void ProcessManager::ScanProcesses() {
	DWORD aProcesses[1024] = { 0 }, cbNeeded = 0, cProcesses = 0;

	// Get the list of process identifiers.
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		trace_debug(L"ProcessManager::UpdateProcessList: Error EnumProcesses");
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	// Save processes
	this->processes.clear();
	for (unsigned int i = 0; i < cProcesses; i++) {
		this->processes.push_back(aProcesses[i]);
	}
}

std::vector<DWORD> ProcessManager::GetPidListByName(std::string procName)
{
	this->ScanProcesses();

	if (procName.compare("") == 0) {
		//TODO? handle error
	}

	auto result = std::vector<DWORD>();
	std::string curProcName = "\0";

	for (const DWORD& pid : this->processes) {
		curProcName = Process::GetName(pid);

		if (procName.compare(curProcName) == 0) {
			result.push_back(pid);
		}
	}

	return std::move(result);
}

std::string ProcessManager::GetProcessCommandLine(DWORD pid) {
	MY_PUBLIC_PROCESS_BASIC_INFORMATION procInfo{};
	ULONG guessedSize = sizeof(procInfo);
	ULONG requiredSize = 0;

	auto procHandle = Process::Open(pid, PROC_ALL_ACCESS_RIGHTS);
	//if (!IsValidAndOpen(procHandle)) {
	//	trace_debug(L"open process failed");
	//	return ERROR_GET_COMMAND_LINE;
	//}

	auto queryProcInfoFunc = (_NtQueryInformationProcess) LoadNtFunction(NT_QUERY_PROC_INFO_NAME);
	if (queryProcInfoFunc == ERROR_LOAD_NT_FUNC) {
		//TODO? handle error
		return ERROR_GET_COMMAND_LINE;
	}

	auto status = queryProcInfoFunc(procHandle.Get(),
		ProcessBasicInformation,
		&procInfo,
		guessedSize,
		&requiredSize);


	//while (status == STATUS_INFO_LENGTH_MISMATCH) {
	//	// Update size, realloc info and query again
	//	guessedSize = requiredSize;
	//	procInfo = (MY_PPUBLIC_PROCESS_BASIC_INFORMATION) this->memManager.Realloc(procInfo, guessedSize);
	//	status = queryProcInfoFunc(procHandle.Get(),
	//		ProcessBasicInformation,
	//		procInfo,
	//		guessedSize,
	//		&requiredSize);
	//}

	if (status != STATUS_SUCCESS) {
		//TODO? handle error
		trace_debug(L"NtQueryInformationProcess failed" + std::to_wstring(status));
	}
	
	// taken from: https://stackoverflow.com/questions/6530565/getting-another-process-command-line-in-windows/13408150#13408150
	auto pebRemote = procInfo.PebBaseAddress;
	PEB peb{};
	status = ReadProcessMemory(procHandle.Get(),
		pebRemote,
		&peb,
		sizeof(PEB),
		NULL);
	if (!status) {
		//TODO? handle error
		trace_debug(L"ReadProcessMemory failed reading peb");
	}

	auto paramsRemote = peb.ProcessParameters;
	RTL_USER_PROCESS_PARAMETERS params{};
	status = ReadProcessMemory(procHandle.Get(),
		paramsRemote,
		&params,
		sizeof(RTL_USER_PROCESS_PARAMETERS),
		NULL);
	if (!status) {
		//TODO? handle error
		trace_debug(L"ReadProcessMemory failed reading rtl params");
	}


	auto cmdLineRemote = params.CommandLine.Buffer;
	guessedSize = params.CommandLine.Length;
	auto cmdLine = (PWSTR) this->memManager.Alloc(guessedSize);
	status = ReadProcessMemory(procHandle.Get(),
		cmdLineRemote,
		cmdLine, // command line goes here
		guessedSize,
		NULL);
	if (!status) {
		//TODO? handle error
		trace_debug(L"ReadProcessMemory failed reading command line");
	}

	auto cmd = std::wstring(cmdLine);
	auto result = std::string(cmd.begin(), cmd.end());
	return std::move(result);
}


