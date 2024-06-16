#pragma once

#include "common.hpp"
#include "chunk.hpp"
#include "vm.hpp"
#include "compiler.hpp"
#include <string>
#include <unordered_map>
#include <memory>

class Lang{
private:
    bool _isRepl, isInit = false, canvasMode, compileOK = false,
        runError = false;
    std::unordered_map<std::string, int> globalVarIdxs;
    std::unordered_map<int, Value> globalVarValues;
    std::unique_ptr<Chunk> chunk;
    std::unique_ptr<Compiler> c;
    std::unique_ptr<VM> vm;
public:
    Lang(bool isRepl = false, bool canvasMode = false) 
        : _isRepl(isRepl), canvasMode(canvasMode) 
    {
        // chunk = std::make_unique<Chunk>(globalVarIdxs);
        // c = std::make_unique<Compiler>(*chunk);

        // vm = std::make_unique<VM>(*chunk, globalVarValues);
    }
    ~Lang() = default;
    double getValue(double t, double i, double x, double y);
    InterpretResult interpret(std::string_view source);
    void compileToBytecode(std::string_view source);
    bool allOK() const { return compileOK && !runError; }
};
