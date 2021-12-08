#pragma once

#include "errors.h"
#include <exception>
#include <windows.h>

class MyException : public std::exception
{
private:
	MyErrors errType;
	NTSTATUS status = 0;

public:
	MyException(MyErrors errorType);
	MyException(MyErrors errorType, NTSTATUS status);
	const char* what() const throw();
};

