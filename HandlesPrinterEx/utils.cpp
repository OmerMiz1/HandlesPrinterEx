#include "utils.h"
#include "consts.h"
#include "errors.h"
#include <set>
#include <sstream>

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
	
	// Case negative number
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
	// Disallow negative numbers
	if (str.front() == '-' || !IsNumber(str)) {
		return false;
	}

	// Check for overflow
	auto longlongVal = std::stoull(str.c_str());
	auto longVal = std::stoul(str.c_str());
	if (longlongVal != longVal) {
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
	} else {
		return ArgumentType::InvalidProcessName;
	}
}


// String converts //
std::string ToString(TCHAR* tchar) {
	// Taken from:
	// https://stackoverflow.com/questions/6006319/converting-tchar-to-string-in-c
	if (tchar != nullptr) {
		std::wstring wstr(tchar);
		return std::string(wstr.begin(), wstr.end());
	}

	return "";
}

std::string ToString(PWSTR pstr, int Length) {
	if (pstr != nullptr) {
		return std::string(pstr, &(pstr[Length]));
	}

	return "";
}


// General //
FARPROC LoadNtFunction(std::string funcName) {
	auto modHandle = GetModuleHandleA(NTDLL_NAME);
	if (modHandle == nullptr) {
		throw_exception(ERROR_GET_MODULE_HANDLE);
	}

	auto result = GetProcAddress(modHandle, funcName.c_str());
	if (result == nullptr) {
		throw_exception(ERROR_GET_PROC_ADDR);
	}

	return result;
}
