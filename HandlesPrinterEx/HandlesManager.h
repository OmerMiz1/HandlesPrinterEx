#pragma once

#include "MyException.h"
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

	void ScanSystemHandles() throw(MyException);
	template<typename T> T GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType) throw(MyException);  //TODO

public:
	HandlesManager();
	~HandlesManager();

	std::vector<SYSTEM_HANDLE> GetProcessHandles(DWORD pid) throw(MyException);
	std::string GetHandleName(SYSTEM_HANDLE handle) throw(MyException);
	std::string GetHandleType(SYSTEM_HANDLE handle) throw(MyException);
	ULONG GetHandlePointerCount(SYSTEM_HANDLE handle) throw(MyException);
	ULONG GetHandleCount(SYSTEM_HANDLE handle) throw(MyException);

	static SmartHandle Duplicate(const SmartHandle& procHandle, HANDLE handleToCopy) throw(MyException);
	static void EnableDebugPrivilege(); //TODO? remove \ mark private
};

template POBJECT_NAME_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType);
template POBJECT_TYPE_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType);
template MY_PPUBLIC_OBJECT_BASIC_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType);


