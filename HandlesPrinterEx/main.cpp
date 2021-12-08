#include "HandlesPrinter.h"

int main(int argc, char** argv) {
	auto printer = HandlesPrinter();
	printer.PrintProcHandles(argc, argv);
}