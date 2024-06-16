#pragma once

#include "chunk.hpp"
#include <string>

void disassembleChunk(Chunk& chunk, std::string_view name);
int disassembleInstruction(Chunk& chunk, int offset);