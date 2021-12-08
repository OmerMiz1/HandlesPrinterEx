#pragma once

#include "errors.h"
#include <exception>
#include <windows.h>
#include <string>

// Class note:
/**
* Generate MyException using the "throw_exception"
* or "throw_exception_with_status" macro (utils.h).
* 
* This way it will also capture the line and function
* of which the excption was thrown.
* 
* Exception types are defined in errors.h in 
* MyError enum.
*/

class MyException : public std::exception
{
private:
	MyError errType;
	std::wstring funcName = L"\0";
	int lineNum = -1;
	NTSTATUS status = 0;

public:
	MyException(MyError errorType);
	MyException(MyError errorType, NTSTATUS status);
	MyException(std::wstring funcName, int lineNum, MyError errorType);
	MyException(std::wstring funcName, int lineNum, MyError errorType, NTSTATUS status);

	/**
	* Returns a msg with the reason for the exception, incase
	* running in debug it also prints it to debug console.
	*/
	const char* what() const throw();
};

