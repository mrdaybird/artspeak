// external library headers
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <string>

// internal library headers
#include "Runner.hpp"

int main(int argc, char ** argv){
	#define tokenToString(p) #p
	art::Runner runner{};
	if(argc > 2){
		fmt::println("Usage: artspeak [file] ");
	}else if(argc == 2){
		runner.runFile(argv[1]);
	}else{
		runner.runREPL();
	}
}