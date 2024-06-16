#include "chunk.hpp"
#include <algorithm>
void Chunk::write(uint8_t byte, int line){
	if(lineNumbers.empty() || lineNumbers.back().second < line){
		lineNumbers.push_back({code.size(), line});
	}
	code.push_back(byte);
}

int Chunk::addConstant(Value value){
	constants.push_back(value);
	return constants.size()-1;
}

int Chunk::writeConstantOp(Value value, int line){
	int constantIdx = addConstant(value);
	if(constantIdx < 256){
		write(OP_CONSTANT, line);
		write((uint8_t)constantIdx, line);
	}else if(constantIdx < STACK_MAX){
		write(OP_CONSTANT_LONG, line);
		write((uint8_t)(constantIdx >> 16), line);
		write((uint8_t)(constantIdx >>  8), line);
		write((uint8_t) constantIdx		  , line);
	}else{
		return -1; // STACK_OVERFLOW
	}
	return constantIdx;
}

int Chunk::getLine(int offset){
	auto it = std::lower_bound(lineNumbers.begin(), lineNumbers.end(), std::make_pair(offset, 0));
	if(it != lineNumbers.end()){
		return it->second;
	}
	return -1;
}