#include "Process.h"
#include "utils.h"
#include "consts.h"
#include "errors.h"
#include <windows.h>
#include <psapi.h>

SmartHandle Process::Open(DWORD pid) {
	return Process::Open(pid, PROC_ACCESS_RIGHTS);
}

SmartHandle Process::Open(DWORD pid, int accessFlags) {
	auto handle = OpenProcess(accessFlags, FALSE, pid);

	if (handle == INVALID_HANDLE_VALUE) {
		// TODO handle error
		trace_debug(L"OpenProcess has failed");
		throw MyErrors::ERROR_OPEN_PROCESS;
		//return SmartHandle(nullptr); //TODO remove
	}

	return SmartHandle(handle);
}

std::string Process::GetName(DWORD pid) {
	// Taken from: https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes
	TCHAR nameBuffer[MAX_PATH] = L"<unknown>\0";
	HMODULE hMod;
	DWORD cbNeeded;

	auto procHandle = Open(pid);
	if (procHandle.Get() == HANDLE_CLOSED_VALUE) {
		//TODO handle error
		trace_debug(L"");
		return TcharToStr(nameBuffer);
	}

	// Get the process name
	if (EnumProcessModules(procHandle.Get(), &hMod, sizeof(hMod), &cbNeeded)) {
		GetModuleBaseName(procHandle.Get(), hMod, nameBuffer, MAX_PATH);
	}
	else {
		trace_debug(L"EnumProcessModules failed");  //TODO? handle error
	}

	return TcharToStr(nameBuffer);
}