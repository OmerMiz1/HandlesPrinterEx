#pragma once

#include <windows.h>
#include <winternl.h>

typedef enum class ArgumentType {
	PID = 0,
	ProcessName,
	InvalidArgumentCount,
	InvalidProcessName
};

/*
Info for enum comes from both:
https://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ob/obquery/class.htm
https://forums.codeguru.com/showthread.php?423112-RESOLVED-W2K-How-to-get-process-handles-details
*/
typedef enum class ObjectInformationClass : ULONG {
	ObjectBasicInformation = 0,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectTypesInformation,
	ObjectHandleFlagInformation,
	ObjectSessionInformation,
	ObjectSessionObjectInformation,
	MaxObjectInfoClass
};

//TODO remove unecessary types
// Taken from: https://github.com/SinaKarvandi/Process-Magics/blob/master/EnumAllHandles/EnumAllHandles/EnumAllHandles.cpp
typedef _SYSTEM_INFORMATION_CLASS SYSTEM_INFORMATION_CLASS;
typedef _UNICODE_STRING UNICODE_STRING, * PUNICODE_STRING;

typedef struct _OBJECT_NAME_INFORMATION
{
	UNICODE_STRING Name;

} OBJECT_NAME_INFORMATION, * POBJECT_NAME_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;
	ULONG Reserved[22];    // reserved for internal use
} OBJECT_TYPE_INFORMATION, * POBJECT_TYPE_INFORMATION;

typedef NTSTATUS(NTAPI* _NtQuerySystemInformation)(
	ULONG SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);

typedef NTSTATUS(NTAPI* _NtDuplicateObject)(
	HANDLE SourceProcessHandle,
	HANDLE SourceHandle,
	HANDLE TargetProcessHandle,
	PHANDLE TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG Attributes,
	ULONG Options
	);

typedef NTSTATUS(NTAPI* _NtQueryObject)(
	HANDLE ObjectHandle,
	ULONG ObjectInformationClass,
	PVOID ObjectInformation,
	ULONG ObjectInformationLength,
	PULONG ReturnLength
	);

typedef NTSTATUS (NTAPI* _NtQueryInformationProcess)(  //TODO make sure NTAPI* works
	HANDLE ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID ProcessInformation,
	ULONG ProcessInformationLength,
	PULONG ReturnLength
);

typedef struct _SYSTEM_HANDLE
{
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG HandleCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef enum _POOL_TYPE
{
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS
} POOL_TYPE, * PPOOL_TYPE;

//typedef struct _OBJECT_TYPE_INFORMATION
//{
//	UNICODE_STRING Name;
//	ULONG TotalNumberOfObjects;
//	ULONG TotalNumberOfHandles;
//	ULONG TotalPagedPoolUsage;
//	ULONG TotalNonPagedPoolUsage;
//	ULONG TotalNamePoolUsage;
//	ULONG TotalHandleTableUsage;
//	ULONG HighWaterNumberOfObjects;
//	ULONG HighWaterNumberOfHandles;
//	ULONG HighWaterPagedPoolUsage;
//	ULONG HighWaterNonPagedPoolUsage;
//	ULONG HighWaterNamePoolUsage;
//	ULONG HighWaterHandleTableUsage;
//	ULONG InvalidAttributes;
//	GENERIC_MAPPING GenericMapping;
//	ULONG ValidAccess;
//	BOOLEAN SecurityRequired;
//	BOOLEAN MaintainHandleCount;
//	USHORT MaintainTypeList;
//	POOL_TYPE PoolType;
//	ULONG PagedPoolUsage;
//	ULONG NonPagedPoolUsage;
//} OBJECT_TYPE_INFORMATION, * POBJECT_TYPE_INFORMATION;

//TODO change to _OBJECT_BASIC_INFORMATION
typedef struct MY_PUBLIC_OBJECT_BASIC_INFORMATION {
	ULONG Attributes;
	ACCESS_MASK GrantedAccess;
	ULONG HandleCount;
	ULONG PointerCount;
	ULONG Reserved[10];    // reserved for internal use
} *MY_PPUBLIC_OBJECT_BASIC_INFORMATION;

typedef struct _MY_PUBLIC_PROCESS_BASIC_INFORMATION {
	NTSTATUS ExitStatus;
	PPEB PebBaseAddress;
	ULONG_PTR AffinityMask;
	KPRIORITY BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
} MY_PUBLIC_PROCESS_BASIC_INFORMATION, *MY_PPUBLIC_PROCESS_BASIC_INFORMATION;