#pragma once

#include "HandlesManager.h"
#include <string>
#include <set>

class HandlesPrinter
{
private:
	HandlesManager* handleMan;  //TODO handle error (CTOR)
	std::set<std::string> allowedTypes;
	bool multiProcMode;

	/**
	* Prints the header of a process, incase of multiple,
	* it also prints process's cmd.
	* 
	* params:
	* pid: the process id to print
	* multiProcMode: determines if cmd will be printed,
	*				 for use in multi process print mode.
	*/
	static void PrintHeader(DWORD pid, bool multiProcMode);
	/**
	* Prints system handle's name, type (if allowed),
	* pointers count and handles count.
	* 
	* params:
	* sysHandle: the handle whos info is to be printed.
	*/
	void PrintHandle(SYSTEM_HANDLE sysHandle) const;
	/**
	* Utility function, returns wether the type is allowed.
	*/
	bool IsAllowedType(std::string type) const;

public:
	HandlesPrinter();
	~HandlesPrinter();

	/**
	* Given process id, prints its handles information.
	* 
	* params:
	* pid: process id
	*/
	void PrintProcHandles(DWORD pid);
	/**
	* Prints process handles by given process's name.
	* 
	* params:
	* processName: the name of the process
	*/
	void PrintProcHandles(std::string processName);
	/**
	* Validates & parses arguments, calls relevant functions
	* if everything is valid or print relevant error.
	*/
	void PrintProcHandles(int argCount, char** args);
};
