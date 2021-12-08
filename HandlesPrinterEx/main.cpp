#include "utils.h"
#include "HandlesPrinter.h"
#include "consts.h"
#include <iostream>

int main(int argc, char** argv) {
	ArgumentType argType = ParseArgType(argc, argv);
	auto printer = HandlesPrinter();

	switch (argType) {
		case ArgumentType::PID: {
			DWORD pid = std::stoul(argv[1]);
			printer.PrintProcHandles(pid, false);
			break;
		}
		case ArgumentType::ProcessName: {
			std::string procName = std::string(argv[1]);
			printer.PrintProcHandles(procName);
			break;
		}
		case ArgumentType::InvalidArgumentCount: {
			std::cout << "Invalid arguments count, 1 argument expected!" << std::endl;
			break;
		}
		case ArgumentType::InvalidProcessName: {
			std::cout << "Invalid argument name, process name can't contain the follwing chars: " << PROC_NAME_BAD_CHARS << std::endl;
			break;
		}
		default: {
			std::cout << "Unknown argument error" << std::endl;
			break;
		}
	}

	return 0;
}