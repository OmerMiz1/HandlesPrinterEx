#pragma once

#include <windows.h>

/**
* Class made to make sure that handles are being closed.
*/
class SmartHandle
{
private:
	HANDLE handle;

public:
	SmartHandle(HANDLE handle);
	~SmartHandle();

	HANDLE Get() const;
};