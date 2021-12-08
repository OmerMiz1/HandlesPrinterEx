#pragma once

#include "MemoryManager.h"
#include "SmartHandle.h"
#include "types.h"
#include <vector>
#include <string>

class HandlesManager
{
private:
	MemoryManager* memManager;
	std::vector<SYSTEM_HANDLE> handles;

	/**
	* Scans entire system handles and stores in handles member.
	* 
	* May throw MyException (see errors.h and MyException class).
	*/
	void ScanSystemHandles() throw();
	/**
	* Generic function that calls NtQueryObject function.
	* Being used to get a handle's:
	*	Name
	*	Type
	*	Pointers count
	*	Handles count
	* 
	* params:
	* handle: the system handle to query about.
	* attrType: type of query (see types.h)
	*/
	template<typename T> T GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType) throw();

public:
	HandlesManager();
	~HandlesManager();

	/**
	* Returns a list of handles related to a process id.
	* 
	* May throw MyException (see MyException.h)
	*/
	std::vector<SYSTEM_HANDLE> GetProcessHandles(DWORD pid) throw();
	/**
	* Returns a handle's name using GetHandleInfo<T>.
	*
	* May throw MyException (see MyException.h)
	*/
	std::string GetHandleName(SYSTEM_HANDLE handle) throw();
	/**
	* Returns a handle's type using GetHandleInfo<T>.
	*
	* May throw MyException (see MyException.h)
	*/
	std::string GetHandleType(SYSTEM_HANDLE handle) throw();
	/**
	* Returns a handle's pointer count using GetHandleInfo<T>.
	*
	* May throw MyException (see MyException.h)
	*/
	ULONG GetHandlePointerCount(SYSTEM_HANDLE handle) throw();
	/**
	* Returns a handle count using GetHandleInfo<T>.
	*
	* May throw MyException (see MyException.h)
	*/
	ULONG GetHandleCount(SYSTEM_HANDLE handle) throw();

	/**
	* Duplicates a handle using DuplicateHandle (winapi).
	* See MSDN for remarks.
	*/
	static SmartHandle Duplicate(const SmartHandle& procHandle, HANDLE handleToCopy) throw();
	static void EnableDebugPrivilege(); //TODO? remove \ mark private
};

// Template declarations for GetHandleInfo<T>
template POBJECT_NAME_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE, ObjectInformationClass);
template POBJECT_TYPE_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE, ObjectInformationClass);
template MY_PPUBLIC_OBJECT_BASIC_INFORMATION HandlesManager::GetHandleInfo(SYSTEM_HANDLE, ObjectInformationClass);


