#include "MyException.h"
#include "utils.h"
#include <string>

MyException::MyException(MyErrors errorType) {
	this->errType = errorType;
}

MyException::MyException(MyErrors errorType, NTSTATUS status) {
	this->errType = errorType;
	this->status = status;
}

const char* MyException::what() const throw() {
	std::wstring msg;

	switch (this->errType) {
		case ERROR_OPEN_PROCESS: {
			msg = L"OpenProcess";
			break;
		}
		case ERROR_ENUM_PROCESSES: {
			msg = L"EnumProcesses";
			break;
		}
		case ERROR_ENUM_PROC_MODULES: {
			msg = L"EnumProcessModules";
			break;
		}
		case ERROR_GET_MODULE_HANDLE: {
			msg = L"GetModuleHandle";
			break;
		}
		case ERROR_GET_PROC_ADDR: {
			msg = L"GetProcAddress";
			break;
		}
		case ERROR_QUERY_SYS_INFO: {
			msg = L"NyQuerySystemInformation";
			break;
		}
		case ERROR_QUERY_PROC_INFO: {
			msg = L"NtQueryInformationProcess";
			break;
		}
		case ERROR_QUERY_OBJ: {
			msg = L"NtQueryObject";
			break;
		}
		case ERROR_DUPLICATE_HANDLE: {
			msg = L"DuplicateHandle";
			break;
		}
		case ERROR_READ_PROC_MEM: {
			msg = L"ReadProcessMemory";
			break;
		}
		case ERROR_VIRTUAL_ALLOC: {
			msg = L"VirtualAlloc";
			break;
		}
		case ERROR_VIRTUAL_FREE_NULLPTR: {
			msg = L"VirtualFree received nullptr so it";
			break;
		}
		case ERROR_VIRTUAL_FREE: {
			msg = L"VirtualFree";
			break;
		}
		default: {
			trace_debug(L"UNKNOWN ERROR");
			return "UNKNOWN ERROR";
		}
	}

	msg += L" failed!";

	if (status != 0) {
		msg += L" Status code: " + std::to_wstring(this->status);
	}

	else if (GetLastError() != 0) {
		msg += L" Error code: " + std::to_wstring(GetLastError());
	}
	
	trace_debug(msg);
	return std::move(std::string(msg.begin(), msg.end()).c_str());
}
