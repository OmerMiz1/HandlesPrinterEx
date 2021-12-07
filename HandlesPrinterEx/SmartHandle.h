#pragma once

#include <windows.h>

class SmartHandle
{
private:
	HANDLE handle = nullptr;

public:
	SmartHandle(HANDLE handle);
	~SmartHandle();

	HANDLE Get() const;
};