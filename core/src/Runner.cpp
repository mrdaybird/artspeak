#include <vector>
#include <fstream>
#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>

#include "Runner.hpp"
#include "Scanner.hpp"
namespace art{
void Runner::run(std::string source){
	art::Scanner sc{*this, source};
	std::vector<art::Token> tokens = sc.scanTokens();
	for(const auto& token : tokens){
		fmt::println("{}", token);
	}
}
void Runner::runFile(char* fileName){
	std::ifstream file(fileName);
	std::string source = "";
	while(file.is_open() && file.good()){
		std::string buf;
		file >> buf;
		source.append(buf);
	}
	fmt::println(source);
	run(std::move(source));
	if(hadError) exit(65);
}
void Runner::runREPL(){
	while(1){
		fmt::print(">> ");
		char src[512];
		std::cin.getline(src, 512);
		run(std::move(src));
		hadError = false;
	}
}
void Runner::error(int line, int col, std::string message){
	report(line, col, "", message);
}
void Runner::report(int line, int col, std::string where, std::string message){
	fmt::print(fg(fmt::color::crimson), "[line {}, col {}] Error{}: {}\n", line, col, where, message);
	hadError = true;
}
}