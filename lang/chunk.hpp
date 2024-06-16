#pragma once

#include "common.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

class Chunk{
public:
	std::vector<uint8_t> code;
	std::vector<Value> constants;
	std::vector<std::pair<int, int>> lineNumbers;
	std::unordered_map<std::string, int> &globals;
public:
	Chunk(std::unordered_map<std::string, int> &globalVarIdxs) 
		: globals(globalVarIdxs)
	{ }
	~Chunk() = default;
	void write(uint8_t byte, int line);
	int addConstant(Value value);
	int writeConstantOp(Value value, int line);
	int getLine(int offset);
};