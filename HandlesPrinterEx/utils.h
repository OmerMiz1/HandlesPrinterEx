#pragma once

#include "types.h"
#include "SmartHandle.h"
#include <string>
#include <windows.h>

// taken from: https://stackoverflow.com/questions/29049686/is-there-a-better-way-to-pass-formatted-output-to-outputdebugstring
#ifdef  _DEBUG
#define trace_debug(wmsg) MyOutputDebugStringW(__FUNCTIONW__, __LINE__, wmsg)
#else
#define trace_debug
#endif //  _DEBUG

std::string TcharToStr(TCHAR* tchar);
std::string PwstrToStr(PWSTR pwstr, int length);

bool IsValidProcessNameChar(char c);
bool IsValidProcessName(std::string str);  // TODO remove, unnecassery
bool IsValidPid(std::string str);
ArgumentType ParseArgType(int argsCount, char** args);

void MyOutputDebugStringW(std::wstring funcName, int line, std::wstring msg);  //TODO debug
FARPROC LoadNtFunction(std::string funcName);
bool IsValidAndOpen(const SmartHandle& handle);

