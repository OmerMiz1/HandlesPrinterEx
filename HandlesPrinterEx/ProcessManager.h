#pragma once

#include "MemoryManager.h"
#include "MyException.h"
#include "SmartHandle.h"
#include <vector>
#include <string>
#include <windows.h>

class ProcessManager
{
private:
	ProcessManager(); // Static class
	/**
	* Returns a list of processes running in the system (pids).
	* 
	* May throw MyException (see MyException.h).
	*/
	static std::vector<DWORD> ScanProcesses() throw(MyException);

public:
	/**
	* Given a process's name, returns a list of pids for
	* active processes with this name.
	*/
	static std::vector<DWORD> GetPidListByName(std::string name) throw(MyException);
	/**
	* Returns a process's command line given its id.
	* 
	* May throw MyException (see MyException.h).
	*/
	static std::string GetProcessCommandLine(DWORD pid) throw(MyException);
	/**
	* Opens a a process and returns a smart handle
	* containing its handle.
	*
	* May throw MyException (see MyException.h).
	*/
	static SmartHandle Open(DWORD pid) throw(MyException);
	static SmartHandle Open(DWORD pid, int accessFlags) throw(MyException);
	/**
	* Returns a process's name.
	*
	* May throw MyException (see MyException.h).
	*/
	static std::string GetName(DWORD pid) throw(MyException);
};

