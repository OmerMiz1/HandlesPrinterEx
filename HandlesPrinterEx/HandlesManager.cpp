#include "HandlesManager.h"
#include "ProcessManager.h"
#include "utils.h"
#include "types.h"
#include "consts.h"
#include "errors.h"

HandlesManager::HandlesManager() {
	this->memManager = new MemoryManager();
	this->ScanSystemHandles();
	//this->EnableDebugPrivilege(); //TODO? remove
}

HandlesManager::~HandlesManager() {
	if (this->memManager != nullptr) {
		delete this->memManager;
	}
}

void HandlesManager::ScanSystemHandles() {
	// Declaration and Initialization of Variables
	PSYSTEM_HANDLE_INFORMATION handleInfo = NULL;
	ULONG guessedSize = sizeof(*handleInfo);
	ULONG requiredSize = 0;

	// Allocate Memory
	handleInfo = (PSYSTEM_HANDLE_INFORMATION) this->memManager->Alloc((SIZE_T)guessedSize);
	auto querySysInfoFunc = (_NtQuerySystemInformation) LoadNtFunction(NT_QUERY_SYSTEM_INFO_NAME);

	// Loop until the function succeeds
	NTSTATUS status = querySysInfoFunc(SystemHandleInformation, handleInfo, guessedSize, &requiredSize);
	while (status == STATUS_INFO_LENGTH_MISMATCH) {
		// Update size, realloc info and query again
		guessedSize = requiredSize;
		handleInfo = (PSYSTEM_HANDLE_INFORMATION) this->memManager->Realloc(handleInfo, guessedSize);
		status = querySysInfoFunc(SystemHandleInformation, handleInfo, guessedSize, &requiredSize);
	}

	if (status != STATUS_SUCCESS) {
		trace_debug(L"bad status code: " + std::to_wstring(status));
		throw MyErrors::ERROR_QUERY_SYS_INFO;
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
		trace_debug(L"DuplicateHandle failed");
		throw MyErrors::ERROR_DUPLICATE_HANDLE;
	} else if (status == 0x1) {  //TODO? remove? what does it mean? (they had it for the case procHandle is for current process)
		Sleep(10);
	}

	return SmartHandle(handleBuffer);
}

//TODO? force T to be ptr type
template<typename T>
T HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType) {
	// guessedSize: some info classes requires FIXED size, more info at:
	// https://stackoverflow.com/questions/46108382/ntquerysysteminformation-returns-24-error-bad-length
	T objectInfo = nullptr; // TODO? init with {}
	ULONG guessedSize = sizeof(*objectInfo);
	ULONG requiredSize = 0;

	auto procHandle = ProcessManager::Open(handle.ProcessId, PROCESS_DUP_HANDLE);
	auto dupHandle = HandlesManager::DuplicateHandle(procHandle, (HANDLE) handle.Handle);
	objectInfo = (T) this->memManager->Alloc(guessedSize);
	auto queryObjectFunc = (_NtQueryObject)LoadNtFunction(NT_QUERY_OBJECT_NAME);

	NTSTATUS status = queryObjectFunc(dupHandle.Get(), (ULONG) attrType, objectInfo, guessedSize, &requiredSize);
				
	while (status != STATUS_SUCCESS/*status == STATUS_INFO_LENGTH_MISMATCH || status == STATUS_BUFFER_OVERFLOW*/) {
		guessedSize = requiredSize;
		objectInfo = (T) this->memManager->Realloc(objectInfo, guessedSize);
		status = queryObjectFunc(dupHandle.Get(), (ULONG) attrType, objectInfo, guessedSize, &requiredSize);
	}

	if (status != STATUS_SUCCESS) {
		trace_debug(L"bad status code: " + std::to_wstring(status));
		throw MyErrors::ERROR_QUERY_OBJ;
	}
	return objectInfo;
}

std::string HandlesManager::GetHandleName(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectNameInformation;

	try {
		auto objectInfo = this->GetHandleInfo<POBJECT_NAME_INFORMATION>(handle, queryAttrType);
		
		if (objectInfo->Name.Buffer == nullptr) {
			return UNKNOWN_HANDLE_NAME;
		}

		return PwstrToStr(objectInfo->Name.Buffer, objectInfo->Name.Length);
	}
	catch (...) { //TODO make specific
		return UNKNOWN_HANDLE_NAME;
	}
}

std::string HandlesManager::GetHandleType(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectTypeInformation;
	
	try {
		auto objectInfo = this->GetHandleInfo<POBJECT_TYPE_INFORMATION>(handle, queryAttrType);

		if (objectInfo->TypeName.Buffer == nullptr) {
			return UNKNOWN_HANDLE_TYPE;
		}

		return PwstrToStr(objectInfo->TypeName.Buffer, objectInfo->TypeName.Length);
	}
	catch (...) { //TODO make specific
		return UNKNOWN_HANDLE_TYPE;
	}
}

ULONG HandlesManager::GetHandlePointerCount(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectBasicInformation;
	
	try {
		auto objectInfo = this->GetHandleInfo<MY_PPUBLIC_OBJECT_BASIC_INFORMATION>(handle, queryAttrType);
		return objectInfo->PointerCount;
	}
	catch (...) { //TODO make specific
		return UNKNOWN_HANDLE_PTR_COUNT;
	}
}

ULONG HandlesManager::GetHandleCount(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectBasicInformation;
	
	try {
		auto objectInfo = this->GetHandleInfo<MY_PPUBLIC_OBJECT_BASIC_INFORMATION>(handle, queryAttrType);
		return objectInfo->HandleCount;
	}
	catch (...) { //TODO make more specific
		return UNKNOWN_HANDLES_COUNT;
	}


}

void HandlesManager::EnableDebugPrivilege() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tokenPriv{};
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