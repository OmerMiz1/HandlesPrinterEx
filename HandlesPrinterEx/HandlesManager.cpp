#include "HandlesManager.h"
#include "ProcessManager.h"
#include "MyException.h"
#include "utils.h"
#include "types.h"
#include "consts.h"
#include "errors.h"

HandlesManager::HandlesManager() {
	this->memManager = new MemoryManager();
	this->ScanSystemHandles();
}

HandlesManager::~HandlesManager() {
	if (this->memManager != nullptr) {
		delete this->memManager;
	}
}

void HandlesManager::ScanSystemHandles() {
	// Declaration and Initialization of Variables
	PSYSTEM_HANDLE_INFORMATION handleInfo = NULL;
	ULONG size = sizeof(*handleInfo);
	ULONG requiredSize = 0;

	// Allocate Memory
	handleInfo = (PSYSTEM_HANDLE_INFORMATION) this->memManager->Alloc((SIZE_T)size);
	auto querySysInfoFunc = (_NtQuerySystemInformation) LoadNtFunction(NT_QUERY_SYSTEM_INFO_NAME);

	// Update size, realloc info and query again untill success
	NTSTATUS status = querySysInfoFunc(SystemHandleInformation, handleInfo, size, &requiredSize);
	while (status == STATUS_INFO_LENGTH_MISMATCH) {
		size = requiredSize;
		handleInfo = (PSYSTEM_HANDLE_INFORMATION) this->memManager->Realloc(handleInfo, size);
		status = querySysInfoFunc(SystemHandleInformation, handleInfo, size, &requiredSize);
	}

	if (status != STATUS_SUCCESS) {
		throw_exception_with_status(ERROR_QUERY_SYS_INFO, status);
	}

	// Clear prev handles and store found handles in member
	this->handles.clear();
	auto start = handleInfo->Handles;
	auto end = &(handleInfo->Handles[handleInfo->HandleCount]);
	this->handles = std::vector<SYSTEM_HANDLE>(start, end);
}

std::vector<SYSTEM_HANDLE> HandlesManager::GetProcessHandles(DWORD pid) {
	auto result = std::vector<SYSTEM_HANDLE>();
	
	for (auto const &handle : this->handles) {
		if (handle.ProcessId == pid) { // Add relevant handles only
			result.emplace_back(handle);
		}
	}

	return std::move(result);
}

SmartHandle HandlesManager::Duplicate(const SmartHandle& procHandle, HANDLE handleToCopy) {
	HANDLE handleBuffer = NULL;

	auto status = DuplicateHandle(procHandle.Get(),
		handleToCopy,
		GetCurrentProcess(),
		&handleBuffer,
		PROCESS_ALL_ACCESS,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	if (status == STATUS_INVALID_HANDLE || handleBuffer == HANDLE_CLOSED_VALUE) {
		throw_exception(ERROR_DUPLICATE_HANDLE);
	} else if (status == 0x1) {
		// In the example i saw, they sleeped incase procHandle was for 
		// the current process. For now it doesnt affect run time too much
		// so ill keep it.
		// TODO clarify
		Sleep(10);
	}

	return SmartHandle(handleBuffer);
}

template<typename T>
T HandlesManager::GetHandleInfo(SYSTEM_HANDLE handle, ObjectInformationClass attrType) {
	// size: some info classes requires FIXED size, more info at:
	// https://stackoverflow.com/questions/46108382/ntquerysysteminformation-returns-24-error-bad-length
	T objectInfo = nullptr;
	ULONG size = sizeof(*objectInfo);
	ULONG requiredSize = 0;

	// Open handle, duplicate it and allocate memory for target object info.
	auto procHandle = ProcessManager::Open(handle.ProcessId, PROCESS_DUP_HANDLE);
	auto dupHandle = HandlesManager::Duplicate(procHandle, (HANDLE) handle.Handle);
	objectInfo = (T) this->memManager->Alloc(size);

	// Load nt query and execute
	auto queryObjectFunc = (_NtQueryObject)LoadNtFunction(NT_QUERY_OBJECT_NAME);
	NTSTATUS status = queryObjectFunc(dupHandle.Get(), (ULONG) attrType, objectInfo, size, &requiredSize);
	
	// Reallocate memory as required specifies and re execute query untill success.
	while (status != STATUS_SUCCESS) {
		size = requiredSize;
		objectInfo = (T) this->memManager->Realloc(objectInfo, size);
		status = queryObjectFunc(dupHandle.Get(), (ULONG) attrType, objectInfo, size, &requiredSize);
	}

	if (status != STATUS_SUCCESS) {
		throw_exception_with_status(ERROR_QUERY_OBJ, status);
	}

	return objectInfo;
}

std::string HandlesManager::GetHandleName(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectNameInformation;
	std::string result = UNKNOWN_HANDLE_NAME;

	try {
		auto objectInfo = this->GetHandleInfo<POBJECT_NAME_INFORMATION>(handle, queryAttrType);
		if (objectInfo->Name.Buffer != nullptr) {
			result = ToString(objectInfo->Name.Buffer, objectInfo->Name.Length);
		}
	}
	catch (MyException e) { //TODO make specific
		e.what();
	}

	return result;
}

std::string HandlesManager::GetHandleType(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectTypeInformation;
	std::string result = UNKNOWN_HANDLE_TYPE;

	try {
		auto objectInfo = this->GetHandleInfo<POBJECT_TYPE_INFORMATION>(handle, queryAttrType);

		if (objectInfo->TypeName.Buffer != nullptr) {
			result = ToString(objectInfo->TypeName.Buffer, objectInfo->TypeName.Length);
		}
	}
	catch (MyException e) { //TODO make specific
		e.what();
	}

	return result;
}

ULONG HandlesManager::GetHandlePointerCount(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectBasicInformation;

	try {
		auto objectInfo = this->GetHandleInfo<MY_PPUBLIC_OBJECT_BASIC_INFORMATION>(handle, queryAttrType);
		return objectInfo->PointerCount;
	}
	catch (MyException e) { //TODO make specific
		e.what();
		return INVALID_HANDLE_PTR_COUNT;
	}
}

ULONG HandlesManager::GetHandleCount(SYSTEM_HANDLE handle) {
	auto queryAttrType = ObjectInformationClass::ObjectBasicInformation;
	
	try {
		auto objectInfo = this->GetHandleInfo<MY_PPUBLIC_OBJECT_BASIC_INFORMATION>(handle, queryAttrType);
		return objectInfo->HandleCount;
	}
	catch (MyException e) { //TODO make more specific
		e.what();
		return INVALID_HANDLES_COUNT;
	}


}