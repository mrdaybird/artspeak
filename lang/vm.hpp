#pragma once

#include "common.hpp"
#include "chunk.hpp"
#include <vector>

class VM{
private:
	Chunk& chunk;
	bool isInit = false;
	std::vector<uint8_t>::iterator ip;
	std::unordered_map<int, Value> &globals;
public:
	std::vector<Value> stack;
public:
	VM(Chunk &chunk, std::unordered_map<int, Value> &globalVarValues) 
		: chunk(chunk), globals(globalVarValues)
	{
		/* nothing to do.*/
	}

	void init(){
		ip = chunk.code.begin();
	}

	~VM() = default;
	InterpretResult run();
public:
	void push(Value value);
	Value pop();
	Value peek(int offset);
	bool getVariable(int idx);
	bool setVariable(int idx, Value val);
	void defineVariable(int idx, Value val);
};