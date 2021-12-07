#pragma once

#include "utils.h"
#include "HandlesPrinter.h"
#include <iostream>

int main(int argc, char** argv) {
	ArgumentType argType = ParseArgType(argc, argv);
	auto printer = HandlesPrinter();

	switch (argType) {
		case ArgumentType::PID: {
			DWORD pid = std::stoul(argv[1]);
			printer.Print(pid, false);
			break;
		}
		case ArgumentType::ProcessName: {
			std::string procName = std::string(argv[1]);
			printer.Print(procName);
			break;
		}
		case ArgumentType::InvalidArgumentCount: {
			std::cout << "Invalid arguments count, 1 argument expected!" << std::endl;
			break;
		}
		default: {
			std::cout << "Unknown argument error" << std::endl;
			break;
		}
	}

	return 0;
}