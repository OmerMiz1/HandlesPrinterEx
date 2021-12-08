#include "MyException.h"
#include "utils.h"
#include <sstream>

MyException::MyException(MyError errorType) {
	this->errType = errorType;
}

MyException::MyException(MyError errorType, NTSTATUS status) {
	this->errType = errorType;
	this->status = status;
}

MyException::MyException(std::wstring funcName, int lineNum, MyError errorType) {
	this->errType = errorType;
	this->funcName = funcName;
	this->lineNum = lineNum;
}

MyException::MyException(std::wstring funcName, int lineNum, MyError errorType, NTSTATUS status) {
	this->errType = errorType;
	this->funcName = funcName;
	this->lineNum = lineNum;
	this->status = status;
}

const char* MyException::what() const throw() {
	std::wstringstream stream;

	switch (this->errType) {
		case ERROR_OPEN_PROCESS: {
			stream << L"OpenProcess";
			break;
		}
		case ERROR_ENUM_PROCESSES: {
			stream << L"EnumProcesses";
			break;
		}
		case ERROR_ENUM_PROC_MODULES: {
			stream << L"EnumProcessModules";
			break;
		}
		case ERROR_GET_MODULE_HANDLE: {
			stream << L"GetModuleHandle";
			break;
		}
		case ERROR_GET_PROC_ADDR: {
			stream << L"GetProcAddress";
			break;
		}
		case ERROR_QUERY_SYS_INFO: {
			stream << L"NyQuerySystemInformation";
			break;
		}
		case ERROR_QUERY_PROC_INFO: {
			stream << L"NtQueryInformationProcess";
			break;
		}
		case ERROR_QUERY_OBJ: {
			stream << L"NtQueryObject";
			break;
		}
		case ERROR_DUPLICATE_HANDLE: {
			stream << L"DuplicateHandle";
			break;
		}
		case ERROR_READ_PROC_MEM: {
			stream << L"ReadProcessMemory";
			break;
		}
		case ERROR_VIRTUAL_ALLOC: {
			stream << L"VirtualAlloc";
			break;
		}
		case ERROR_VIRTUAL_FREE_NULLPTR: {
			stream << L"VirtualFree received nullptr so it";
			break;
		}
		case ERROR_VIRTUAL_FREE: {
			stream << L"VirtualFree";
			break;
		}
		default: {
			trace_debug(L"UNKNOWN ERROR");
			return "UNKNOWN ERROR";
		}
	}

	stream << L" failed!";

	if (status != 0) {
		stream << L" status code: " << this->status << L" ";
	} else if (GetLastError() != 0) {
		stream << L" error code: " << GetLastError() << L" ";
	}

	stream << L"( " << this->funcName;
	stream << L" line: " << this->lineNum << L").\t";
	
	// Print to debug
	auto wmsg = stream.str();
	trace_debug(wmsg.c_str());

	// Convert to char* and return
	auto msg = std::string(wmsg.begin(), wmsg.end());
	return std::move(msg.c_str());
}
