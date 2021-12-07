#include "utils.h"
#include "consts.h"
#include "errors.h"

bool IsValidProcessNameChar(char c) {
	//TODO implement
	//https://stackoverflow.com/questions/43629363/how-to-check-if-a-string-contains-a-char
	return true;
}

bool IsValidProcessName(std::string str) {
	for (auto c : str) {
		if (!IsValidProcessNameChar(c)) {
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
		return ArgumentType::InvalidArgument;
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
		result += "\0";
		return std::move(result);
	}
	return "-";

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