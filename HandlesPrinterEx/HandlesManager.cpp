#include "HandlesManager.h"
#include "utils.h"
#include "types.h"
#include "consts.h"
#include "Process.h"

HandlesManager::HandlesManager() {
	this->ScanSystemHandles();
	//this->EnableDebugPrivilege(); //TODO? remove
}

void HandlesManager::ScanSystemHandles() {
	// Declaration and Initialization of Variables
	PSYSTEM_HANDLE_INFORMATION handleInfo = NULL;
	ULONG guessedSize = sizeof(*handleInfo);
	ULONG requiredSize = 0;

	// Allocate Memory
	handleInfo = (PSYSTEM_HANDLE_INFORMATION) this->memManager.Alloc((SIZE_T)guessedSize);
	auto querySysInfoFunc = (_NtQuerySystemInformation) LoadNtFunction(NT_QUERY_SYSTEM_INFO_NAME);
	if (querySysInfoFunc == ERROR_LOAD_NT_FUNC) {
		//TODO? handle error
		return;
	}

	// Loop until the Function Succeeds
	NTSTATUS status = querySysInfoFunc(SystemHandleInformation, handleInfo, guessedSize, &requiredSize);
	while (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		// Update size, realloc info and query again
		guessedSize = requiredSize;
		handleInfo = (PSYSTEM_HANDLE_INFORMATION)this->memManager.Realloc(handleInfo, guessedSize);
		status = querySysInfoFunc(SystemHandleInformation, handleInfo, guessedSize, &requiredSize);
	}

	if (status != STATUS_SUCCESS) {
		//TODO? handle error
		trace_debug(L"bad status code: " + std::to_wstring(status));
	}

	this->handles.clear();
	auto start = handleInfo->Handles;
	auto end = &(handleInfo->Handles[handleInfo->HandleCount]);
	this->handles = std::vector<SYSTEM_HANDLE>(start, end);
}

std::vector<SYSTEM_HANDLE> HandlesManager::GetProcessHandles(DWORD pid) {
	auto result = std::vector<SYSTEM_HANDLE>();
	
	for (auto const &handle : this->handles) {
		if (handle.ProcessId == pid) {
			//TODO if doesnt work use push_back
			result.emplace_back(handle);
		}
	}

	return std::move(result); //TODO? possible mess up with handles?
}

SmartHandle HandlesManager::DuplicateHandle(const SmartHandle& srcProcHandle, HANDLE srcHandle) {
	HANDLE handleBuffer = NULL;

	auto status = ::DuplicateHandle(srcProcHandle.Get(),
		srcHandle,
		GetCurrentProcess(),
		&handleBuffer,
		PROCESS_ALL_ACCESS,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	if (status == STATUS_INVALID_HANDLE || handleBuffer == HANDLE_CLOSED_VALUE) {
		trace_debug(L"DuplicateHandle failed, dupHandleBuf is nullptr");
		return SmartHandle(nullptr);
	} else if (status == 0x1) {  //TODO? what does it mean? (they had it for the case procHandle is for current process)
		Sleep(10);
	}

	return SmartHandle(handleBuffer);
}

//TODO? force T to be ptr type
template<typename T>
T HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType) {
	T objectInfo = nullptr;

	// guessedSize: some info classes requires FIXED size, more info at:
	// https://stackoverflow.com/questions/46108382/ntquerysysteminformation-returns-24-error-bad-length
	ULONG guessedSize = sizeof(*objectInfo);
	ULONG requiredSize = 0;
	NTSTATUS status = 0;

	//try {
	//	auto procHandle = Process::Open(handle.ProcessId, PROCESS_DUP_HANDLE);
	//	auto dupHandle = HandlesManager::DuplicateHandle(procHandle, (HANDLE)handle.Handle);
	//	objectInfo = (T)this->memManager.Alloc(guessedSize);
	//	auto queryObjectFunc = (_NtQueryObject)LoadNtFunction(NT_QUERY_OBJECT_NAME);
	//}
	//catch (auto e) {
	//	trace_debug(e.what());
	//	return ERROR_GET_OBJ_INFO;
	//}

	auto procHandle = Process::Open(handle.ProcessId, PROCESS_DUP_HANDLE);
	//if (!IsValidAndOpen(procHandle)) {
	//	trace_debug(L"process handle isnt valid");
	//	return ERROR_GET_OBJ_INFO;
	//}
	
	// Duplicate to get real handle (rather than just pseudo-handle)
	// https://stackoverflow.com/questions/2172079/duplicatehandle-why-duplicate-instead-of-just-acquire
	auto dupHandle = HandlesManager::DuplicateHandle(procHandle, (HANDLE) handle.Handle);
	if (!IsValidAndOpen(dupHandle)) {
		trace_debug(L"dupicated handle is invalid");
		return ERROR_GET_OBJ_INFO;
	}

	objectInfo = (T) this->memManager.Alloc(guessedSize);

	auto queryObjectFunc = (_NtQueryObject)LoadNtFunction(NT_QUERY_OBJECT_NAME);
	if (queryObjectFunc == ERROR_LOAD_NT_FUNC) {
		return ERROR_GET_OBJ_INFO;
	}

	status = queryObjectFunc(dupHandle.Get(), (ULONG) attrType, objectInfo, guessedSize, &requiredSize);
				
	while (status == STATUS_INFO_LENGTH_MISMATCH) {
		guessedSize = requiredSize;
		objectInfo = (T)this->memManager.Realloc(objectInfo, guessedSize);
		status = queryObjectFunc(dupHandle.Get(), (ULONG) attrType, objectInfo, guessedSize, &requiredSize);
	}

	if (status != STATUS_SUCCESS) {
		trace_debug(L"bad status code: " + std::to_wstring(status));
	}

	return objectInfo;
}

std::string HandlesManager::GetHandleName(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectNameInformation;
	auto objectInfo = this->GetHandleInfo<POBJECT_NAME_INFORMATION>(handle, queryAttrType);

	if (objectInfo == nullptr) {
		trace_debug(L"handle object returned null");
		return UNKNOWN_HANDLE_NAME;
	}

	return PwstrToStr(objectInfo->Name.Buffer, objectInfo->Name.Length);
}

std::string HandlesManager::GetHandleType(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectTypeInformation;
	auto objectInfo = this->GetHandleInfo<POBJECT_TYPE_INFORMATION>(handle, queryAttrType);

	if (objectInfo == nullptr) {
		trace_debug(L"handle object returned null");
		return UNKNOWN_HANDLE_TYPE;
	}

	return PwstrToStr(objectInfo->TypeName.Buffer, objectInfo->TypeName.Length);
}

ULONG HandlesManager::GetPointerCount(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectBasicInformation;
	auto objectInfo = this->GetHandleInfo<MY_PPUBLIC_OBJECT_BASIC_INFORMATION>(handle, queryAttrType);

	if (objectInfo == nullptr) {
		trace_debug(L"handle object returned null");
		return UNKNOWN_HANDLE_PTR_COUNT;
	}

	return objectInfo->PointerCount;
}

ULONG HandlesManager::GetHandleCount(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectBasicInformation;
	auto objectInfo = this->GetHandleInfo<MY_PPUBLIC_OBJECT_BASIC_INFORMATION>(handle, queryAttrType);

	if (objectInfo == nullptr) {
		trace_debug(L"handle object returned null");
		return UNKNOWN_HANDLES_COUNT;
	}

	return objectInfo->HandleCount;
}

void HandlesManager::EnableDebugPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tokenPriv;
	LUID luidDebug;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) != FALSE) {
		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidDebug) != FALSE)
		{
			tokenPriv.PrivilegeCount = 1;
			tokenPriv.Privileges[0].Luid = luidDebug;
			tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(tokenPriv), NULL, NULL);
		}
	}
}

//std::string HandlesManager::GetHandleName(SYSTEM_HANDLE handle) {
	//HANDLE dupHandleBuf = NULL;
	//POBJECT_NAME_INFORMATION objectName;
	//ULONG guessedSize = 256;
	//ULONG requiredSize = 0;
	//NTSTATUS status = 0;
	//// Check if can get a Handle by OpenProcess
	//auto procHandle = Process::Open(handle.ProcessId, PROCESS_DUP_HANDLE);
	//if (IsValidAndOpen(procHandle)) {
	//	// Try to Duplicate the Target File Handle
	//	status = DuplicateHandle(procHandle.Get(),
	//		(HANDLE) handle.Handle,
	//		GetCurrentProcess(),
	//		&dupHandleBuf,
	//		PROCESS_ALL_ACCESS,
	//		FALSE,
	//		DUPLICATE_SAME_ACCESS);
	//	if (dupHandleBuf == nullptr) {
	//		//TODO handle error
	//		trace_debug(L"DuplicateHandle failed, dupHandleBuf is nullptr");
	//	}
	//	SmartHandle dupHandle = SmartHandle(dupHandleBuf);
	//	if (status == 0x1) {  //TODO? what does it mean?
	//		Sleep(10);
	//	}
	//	if (status != STATUS_INVALID_HANDLE) {
	//		// Check if the Duplicated Handle is Valid and Opened
	//		if (IsValidAndOpen(dupHandle)) {
	//			objectName = (POBJECT_NAME_INFORMATION) this->memMan.Alloc(guessedSize);
	//			auto queryObjectFunc = (_NtQueryObject) LoadNtFunction(NT_QUERY_OBJECT_NAME);
	//			status = queryObjectFunc(dupHandle.Get(), ObjectNameInformation, objectName, guessedSize, &requiredSize);
	//			while (status != 0) {  // more specific test!
	//				guessedSize = requiredSize;
	//				objectName = (POBJECT_NAME_INFORMATION)this->memMan.Realloc(objectName ,guessedSize);
	//				status = queryObjectFunc(dupHandle.Get(), ObjectNameInformation, objectName, guessedSize, &requiredSize);
	//			}
	//			
	//			return PwstrToStr(objectName->Name.Buffer, objectName->Name.Length);
	//		}
	//		else {
	//			// TODO handle error
	//			trace_debug(L"dupicated handle is invalid");
	//		}
	//	}
	//	else {
	//		//TODO handle error
	//		trace_debug(std::to_wstring(status));
	//	}
	//}
	//else {
	//	//TODO handle error
	//	trace_debug(L"process handle isnt valid");
	//	// cout << "Failed to Duplicate : " << (PVOID)Handle.Handle << " With Access Mask of : " <<(PVOID)Handle.GrantedAccess << " Process ID : " << Handle.ProcessId << "\n";
	//}
	//return "-";
//}