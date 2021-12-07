#pragma once

#include <windows.h>
#include <winternl.h>
// TODO NOT MINE
//#define NT_SUCCESS(x) ((x) >= 0)
#define STATUS_SUCCESS 0
#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004
#define SystemHandleInformation 16
//#define ObjectBasicInformation 0  //TODO remove
//#define ObjectNameInformation 1
//#define ObjectTypeInformation 2
//#define ObjectTypesInformation 3
//#define ObjectHandleFlagInformation 4
//#define ObjectSessionInformation 5
//#define ObjectSessionObjectInformation 6
// NOT MINE

#define PROC_ALL_ACCESS_RIGHTS PROCESS_ALL_ACCESS  //TODO remove if not used
#define PROC_ACCESS_RIGHTS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)
#define HANDLE_CLOSED_VALUE nullptr
#define MEM_ALLOC_TYPE (MEM_RESERVE | MEM_COMMIT)
#define MEM_ALLOC_PROTECTION PAGE_READWRITE

constexpr auto NTDLL_NAME = "ntdll.dll\0";
constexpr auto NT_QUERY_OBJECT_NAME = "NtQueryObject\0";
constexpr auto NT_QUERY_SYSTEM_INFO_NAME = "NtQuerySystemInformation\0";
constexpr auto NT_QUERY_PROC_INFO_NAME = "NtQueryInformationProcess\0";
constexpr auto PROC_NAME_UNKNOWN = "<unkown>";  //TODO remove
constexpr auto TRACE_DEBUG_FORMAT = L"Omer's exe failed with error code: %lu. Error message: %s (%s at line %d)\r\n\0";
constexpr auto PROC_NAME_BAD_CHARS = "<>:\"/\\|?*"; // according to: https://stackoverflow.com/questions/1976007/what-characters-are-forbidden-in-windows-and-linux-directory-names


constexpr auto ERROR_LOAD_NT_FUNC = nullptr;
constexpr auto ERROR_GET_OBJ_INFO = nullptr;
constexpr auto ERROR_GET_COMMAND_LINE = nullptr;

constexpr auto UNKNOWN_HANDLE_TYPE = "<Unknown Type>";
constexpr auto UNKNOWN_HANDLE_NAME = "<Unknown Name>";
constexpr auto UNKNOWN_HANDLE_PTR_COUNT = 0;
constexpr auto UNKNOWN_HANDLES_COUNT = 0;
