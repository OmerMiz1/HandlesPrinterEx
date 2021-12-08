#pragma once

#define STATUS_SUCCESS 0
#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004
#define STATUS_BUFFER_OVERFLOW 0x80000005
#define SystemHandleInformation 16

#define PROC_ACCESS_RIGHTS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)
#define HANDLE_CLOSED_VALUE nullptr

constexpr auto NTDLL_NAME = "ntdll.dll\0";
constexpr auto NT_QUERY_OBJECT_NAME = "NtQueryObject\0";
constexpr auto NT_QUERY_SYSTEM_INFO_NAME = "NtQuerySystemInformation\0";
constexpr auto NT_QUERY_PROC_INFO_NAME = "NtQueryInformationProcess\0";
constexpr auto PROC_NAME_BAD_CHARS = "<>:\"/\\|?*";

constexpr auto UNKNOWN_HANDLE_NAME = "<Unknown Name>";
constexpr auto UNKNOWN_HANDLE_POINTER_COUNT_MSG = "<Unknown PCount>";
constexpr auto UNKNOWN_HANDLES_COUNT_MSG = "<Unknown HCount>";
constexpr auto UNKNOWN_HANDLE_TYPE = "";
constexpr auto INVALID_HANDLE_PTR_COUNT = 0;
constexpr auto INVALID_HANDLES_COUNT = 0;
constexpr auto ALLOWED_HANDLE_TYPES = { "Key","Section","File","Directory","Thread","Mutant","Semaphore","Desktop","WindowStation" };