#pragma once

#include "types.h"
#include "SmartHandle.h"
#include "MyException.h"
#include <string>
#include <windows.h>

// taken from:
// https://stackoverflow.com/questions/29049686/is-there-a-better-way-to-pass-formatted-output-to-outputdebugstring
#ifdef  _DEBUG
#define trace_debug(wmsg) MyOutputDebugStringW(__FUNCTIONW__, __LINE__, wmsg)
#else
#define trace_debug
#endif //  _DEBUG

std::string ToString(TCHAR* tchar);
std::string ToString(PWSTR pwstr, int length);

bool IsValidProcessName(std::string str);
bool IsNumber(std::string str);
bool IsValidPid(std::string str);
ArgumentType ParseArgType(int argsCount, char** args);

void MyOutputDebugStringW(std::wstring funcName, int line, std::wstring msg);
FARPROC LoadNtFunction(std::string funcName) throw(MyException);
bool IsValidAndOpen(const SmartHandle& handle);
void MyReadProcessMemory(HANDLE procHandle, LPVOID baseAddr, LPVOID buffer, SIZE_T bufferSize, SIZE_T* requiredSize);


