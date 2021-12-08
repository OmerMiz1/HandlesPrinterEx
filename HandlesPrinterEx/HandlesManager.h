#pragma once

#include "MemoryManager.h"
#include "SmartHandle.h"
#include "types.h"
#include <vector>
#include <string>
#include <windows.h>

class HandlesManager
{
private:
	MemoryManager* memManager;
	std::vector<SYSTEM_HANDLE> handles;  //TODO Free on after scan? free at the end?

	void ScanSystemHandles();
	template<typename T> T GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType) throw(...);  //TODO

public:
	HandlesManager();
	~HandlesManager();

	std::vector<SYSTEM_HANDLE> GetProcessHandles(DWORD pid);
	std::string GetHandleName(SYSTEM_HANDLE handle);
	std::string GetHandleType(SYSTEM_HANDLE handle);
	ULONG GetHandlePointerCount(SYSTEM_HANDLE handle);
	ULONG GetHandleCount(SYSTEM_HANDLE handle);

	static SmartHandle DuplicateHandle(const SmartHandle& srcProcHandle, HANDLE srcHandle);
	static void EnableDebugPrivilege(); //TODO? remove \ mark private
};

template POBJECT_NAME_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType);
template POBJECT_TYPE_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType);
template MY_PPUBLIC_OBJECT_BASIC_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType);


