#include "utils.h"
#include "consts.h"
#include "errors.h"
#include <set>

// Arguments\String parsing //
bool IsValidProcessName(std::string str) {
	auto badChars = std::set<char>();
	auto len = std::strlen(PROC_NAME_BAD_CHARS);

	for (auto i = 0; i < len; i++) {
		badChars.insert(PROC_NAME_BAD_CHARS[i]);
	}
	
	for (auto c : str) {
		if (badChars.count(c) > 0) {
			return false;
		}
	}
	return true;
}

bool IsNumber(std::string str) {
	bool skipFirst = false;
	
	if (str.front() == '-') {
		skipFirst = true;
	}

	for (auto c : str) {
		if (skipFirst) {
			skipFirst = false;
			continue;
		}
		if (!std::isdigit(c)) {
			return false;
		}
	}

	return true;
}

bool IsValidPid(std::string str) {
	if (str.front() == '-' || !IsNumber(str)) {
		return false;
	}

	// Check for overflow
	auto val = std::stoul(str.c_str());
	auto valStr = std::to_string(val);

	if (valStr.compare(str) != 0) {
		return false;
	}
	
	return true;
}

ArgumentType ParseArgType(int argsCount, char** args) {
	if (argsCount != 2) {
		return ArgumentType::InvalidArgumentCount;
	}

	auto argStr = std::string(args[1]);
	if (IsValidPid(argStr)) {
		return ArgumentType::PID;
	} else if (IsValidProcessName(argStr)) {
		return ArgumentType::ProcessName;
	}
	else {
		return ArgumentType::InvalidProcessName;
	}
}


// String converts //
std::string ToString(TCHAR* tchar) {
	// Taken from:
	// https://stackoverflow.com/questions/6006319/converting-tchar-to-string-in-c
	std::wstring wstr(tchar);
	return std::string(wstr.begin(), wstr.end());  //TODO? use move
}

std::string ToString(PWSTR pstr, int Length)
{
	if (pstr != nullptr) {
		auto result = std::string(pstr, &(pstr[Length]));
		return std::move(result);
	}
	return nullptr;

	/*char* str = new char[Length+1];
	
	if (Length <= 0) {
		str[0] = '-\0';
	} else {
		for (int a = 0; a < Length; a++) {
			str[a] = (char)(LPCTSTR) pstr[a];
		}
		str[Length] = '\0';
	}
	auto result = std::string(str);
	free(str);
	return result;*/
}


// General //
void MyOutputDebugStringW(std::wstring funcName, int line, std::wstring msg) {
	wchar_t msgBuffer[1024] = L"\0";
	int len = swprintf_s(msgBuffer, 1024, TRACE_DEBUG_FORMAT, GetLastError(), msg.c_str(), funcName.c_str(), line);

	OutputDebugStringW(msgBuffer); // TODO check len?
}

bool IsValidAndOpen(const SmartHandle & handle) {
	return (handle.Get() != INVALID_HANDLE_VALUE && handle.Get() != NULL);  // TODO why double check? 
}

FARPROC LoadNtFunction(std::string funcName) {
	auto modHandle = GetModuleHandleA(NTDLL_NAME);
	if (modHandle == nullptr) {
		throw MyException(ERROR_GET_MODULE_HANDLE);
	}

	auto result = GetProcAddress(modHandle, funcName.c_str());
	if (result == nullptr) {
		throw MyException(ERROR_GET_PROC_ADDR);
	}

	return result;
}

void MyReadProcessMemory(HANDLE procHandle, LPVOID baseAddr, LPVOID buffer, SIZE_T bufferSize, SIZE_T* requiredSize) {
	auto status = ReadProcessMemory(procHandle, baseAddr, buffer, bufferSize, requiredSize);
	if (status != STATUS_SUCCESS) {
		throw MyException(ERROR_READ_PROC_MEM);
	}
}