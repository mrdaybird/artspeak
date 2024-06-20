#include "debug.hpp"
#include "chunk.hpp"
#include <fmt/core.h>
#include <fmt/color.h>

void disassembleChunk(Chunk& chunk, std::string_view name){
	fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::dim_gray),
		 "===== {} =====\n", name);
	size_t lineNumberIdx = 0;
	for(int offset = 0; offset < (int)chunk.code.size();){
		if(lineNumberIdx < chunk.lineNumbers.size() &&
			chunk.lineNumbers[lineNumberIdx].first == offset){
				fmt::print(fmt::fg(fmt::color::dim_gray),
				"{:4d} ", chunk.lineNumbers[lineNumberIdx].second);
				lineNumberIdx++;
			}
		else fmt::print(fmt::fg(fmt::color::dim_gray),"   | ");
		offset = disassembleInstruction(chunk, offset);
	}
	fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::dim_gray),
	 "=====  end diss!  =====\n");
}

static int simpleInstruction(std::string_view op, int offset){
	fmt::print(fmt::fg(fmt::color::dim_gray), "{}\n", op);
	return offset + 1;
}

static int constantInstruction(std::string_view op, Chunk &chunk, int offset){
	uint8_t idx = chunk.code[offset+1];
	Value constant = chunk.constants[idx];
	fmt::print(fmt::fg(fmt::color::dim_gray), 
		"{:<16s} {:4d} {}\n", op, idx, constant);
	return offset + 2;
}

static int constantLongInstruction(std::string_view op, Chunk &chunk, int offset){
	int idx = chunk.code[offset+1] << 16 | chunk.code[offset+2] << 8| 
				  chunk.code[offset+3];
	Value constant = chunk.constants[idx];
	fmt::print(fmt::fg(fmt::color::dim_gray),
		"{:<16s} {:4d} '{}'\n", op, idx, constant);
	return offset + 4;
}

int disassembleInstruction(Chunk& chunk, int offset){
	fmt::print(fmt::fg(fmt::color::dim_gray), "{:04d} ", offset);

	uint8_t inst = chunk.code[offset];
	switch (inst)
	{
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		case OP_CONSTANT_LONG:
			return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
		case OP_POP:
			return simpleInstruction("OP_POP", offset);
		case OP_PRINT:
			return simpleInstruction("OP_PRINT", offset);
		case OP_ADD:
			return simpleInstruction("OP_ADD", offset);
		case OP_SUB:
			return simpleInstruction("OP_SUB", offset);
		case OP_MUL:
			return simpleInstruction("OP_MUL", offset);
		case OP_DIV:
			return simpleInstruction("OP_DIV", offset);
		case OP_SIN:
			return simpleInstruction("OP_SIN", offset);
		case OP_COS:
			return simpleInstruction("OP_COS", offset);
		case OP_TAN:
			return simpleInstruction("OP_TAN", offset);
		case OP_SQRT:
			return simpleInstruction("OP_SQRT", offset);
		case OP_POW:
			return simpleInstruction("OP_POW", offset);
		case OP_DEFINE_GLOBAL:
			return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
		case OP_DEFINE_GLOBAL_LONG:
			return constantLongInstruction("OP_DEFINE_GLOBAL_LONG", chunk, offset);
		case OP_GET_GLOBAL:
			return constantInstruction("OP_GET_GLOBAL", chunk, offset);
		case OP_GET_GLOBAL_LONG:
			return constantLongInstruction("OP_GET_GLOBAL_LONG", chunk, offset);
		case OP_SET_GLOBAL:
			return constantInstruction("OP_SET_GLOBAL", chunk, offset);
		case OP_SET_GLOBAL_LONG:
			return constantLongInstruction("OP_SET_GLOBAL_LONG", chunk, offset);
		case OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);
		default:
			return simpleInstruction("~UNK~", offset);
	}
}