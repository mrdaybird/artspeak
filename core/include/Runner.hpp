#pragma once

#include <string>

namespace art{
class Runner
{
private:
	bool hadError = false;
	void run(std::string source);
public:
	void runFile(char *fileName);
	void runREPL();
	void error(int line, int col, std::string message);
	void report(int line, int col, std::string where, std::string message);
};
}