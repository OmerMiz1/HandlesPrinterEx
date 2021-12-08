#pragma once

#include "types.h"
#include "SmartHandle.h"
#include "MyException.h"
#include <string>
#include <windows.h>

// taken from:
// https://stackoverflow.com/questions/29049686/is-there-a-better-way-to-pass-formatted-output-to-outputdebugstring
//#ifdef  _DEBUG
#define trace_debug(wmsg) OutputDebugStringW(wmsg)
//#else
//#define trace_debug
//#endif //  _DEBUG

/**
* Made macros for easy exception throw with function name
* and line number as arguments.
* Error type must be of enum MyErrors (types.h).
*/
#define throw_exception(errType){\
	throw MyException(__FUNCTIONW__, __LINE__, errType);\
} 

#define throw_exception_with_status(errType, status){\
	throw MyException(__FUNCTIONW__, __LINE__, errType, status);\
}

/**
* String convertion function returns empty string
* in case value is nullptr.
*/
std::string ToString(TCHAR* tchar);
std::string ToString(PWSTR pwstr, int length);

/**
* Function checks if given string does'nt contain invalid 
* characters for a file\process name.
**/
bool IsValidProcessName(std::string str);
/**
* Checks if given string can represents a number.
**/
bool IsNumber(std::string str);
/**
* Function first checks if given string can be interpreted as
* a number. Than it checks if that number can represent a 64bit 
* address or not (overflow\negative).
**/
bool IsValidPid(std::string str);
/**
* Function validates arguments count and value.
* See ArgumentType in "types.h" for list of possible
* return values.
* 
* Use this function to determine how to parse given arg.
**/
ArgumentType ParseArgType(int argsCount, char** args);

/**
* Loads a functino from ntdll and return it.
* Function may throw exceptions incase of winapi function
* faiulures (see MyErrors in "types.h" for more info).
**/
FARPROC LoadNtFunction(std::string funcName) throw(MyException);
