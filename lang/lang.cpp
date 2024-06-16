#include "lang.hpp"
#include "chunk.hpp"
#include "debug.hpp"
#include "vm.hpp"
#include "compiler.hpp"

#include <cmath>
#include <fmt/core.h>

double Lang::getValue(double t, double i, double x, double y){
    if(!compileOK)
        return 0;

    globalVarValues[0] = t;
    globalVarValues[1] = i;
    globalVarValues[2] = x;
    globalVarValues[3] = y;

    InterpretResult result = vm->run();
    if(result == INTERPRET_OK){
        runError = false;
        return vm->pop();
    }
    else
        runError = true;
    return 0;
}

void Lang::compileToBytecode(std::string_view source){
    chunk = std::make_unique<Chunk>(globalVarIdxs);
    c = std::make_unique<Compiler>(*chunk);
    vm = std::make_unique<VM>(*chunk, globalVarValues);

    globalVarIdxs["t"] = 0;
    globalVarIdxs["i"] = 1;
    globalVarIdxs["x"] = 2;
    globalVarIdxs["y"] = 3;

    compileOK = c->compile(source, true);
}

InterpretResult Lang::interpret(std::string_view source){
    /*
        source(string) -> tokens -> bytecode ->   run
                        |    compiler.c       |   vm.c   
    */
    chunk = std::make_unique<Chunk>(globalVarIdxs);
    c = std::make_unique<Compiler>(*chunk);
    vm = std::make_unique<VM>(*chunk, globalVarValues);
    
    if(!c->compile(source, false))
        return INTERPRET_COMPILE_ERROR;
    
    InterpretResult result = vm->run();

    return result;
}