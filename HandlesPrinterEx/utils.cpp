#include "utils.h"
#include "consts.h"
#include "errors.h"
#include <set>

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

bool IsValidPid(std::string str) {
	for (auto c : str) {
		if (!std::isdigit(c)) {
			return false;
		}
	}

	//TODO? check overflow
	// possible test:
	// 1. converting to unsigned long long 
	// 2. converting back to string
	// 3. comparing origin and converted
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

void MyOutputDebugStringW(std::wstring funcName, int line, std::wstring msg) {
	wchar_t msgBuffer[1024] = L"\0";
	int len = swprintf_s(msgBuffer, 1024, TRACE_DEBUG_FORMAT, GetLastError(), msg.c_str(), funcName.c_str(), line);

	OutputDebugStringW(msgBuffer); // TODO check len?
}

std::string TcharToStr(TCHAR* tchar) {
	// Taken from:
	// https://stackoverflow.com/questions/6006319/converting-tchar-to-string-in-c
	std::wstring wstr(tchar);
	return std::string(wstr.begin(), wstr.end());  //TODO? use move
}

FARPROC LoadNtFunction(std::string funcName) {
	auto modHandle = GetModuleHandleA(NTDLL_NAME);
	if (modHandle == nullptr) {
		trace_debug(L"GetModuleHandleA failed");
		throw MyErrors::ERROR_LOAD_NT_FUNC;
	}

	auto result = GetProcAddress(modHandle, funcName.c_str());
	if (result == nullptr) {
		trace_debug(L"GetProcAddress failed");
		throw MyErrors::ERROR_LOAD_NT_FUNC;
	}

	return result;
}

// Convert Unicode to Printable constant char [String]
std::string PwstrToStr(PWSTR pstr, int Length)
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

bool IsValidAndOpen(const SmartHandle & handle) {
	return (handle.Get() != INVALID_HANDLE_VALUE && handle.Get() != NULL);  // TODO why double check? 
}

void MyReadProcessMemory(HANDLE procHandle, LPVOID baseAddr, LPVOID buffer, SIZE_T bufferSize, SIZE_T* requiredSize) {
	auto status = ReadProcessMemory(procHandle, baseAddr, buffer, bufferSize, requiredSize);
	if (!status) {
		trace_debug(L"ReadProcessMemory failed reading peb");
		throw MyErrors::ERROR_READ_PROC_MEM;
	}
}