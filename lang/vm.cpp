#include "vm.hpp"
#include "debug.hpp"
#include <fmt/core.h>
#include <cmath>

void VM::push(Value value){
	if(stack.size() >= STACK_MAX) return;
	stack.push_back(value);
}

Value VM::pop(){
	Value val = stack.back();
	stack.pop_back();
	return val;
}

Value VM::peek(int offset){
	return *(stack.end()-1-offset);
}

bool VM::getVariable(int idx){
	if(globals.find(idx) == globals.end()){
		// Error reporting how?
		fmt::println(stderr, "Unexpected variable in get op!");
		return false;
	}

	push(globals[idx]);
	return true;
}

bool VM::setVariable(int idx, Value val){
	if(globals.find(idx) == globals.end()) {
		// Error reporting how?
		fmt::println(stderr, "Unexpected variable in set op!");
		return false;
	}

	globals[idx] = val;
	//pop();
	return true;
}

void VM::defineVariable(int idx, Value val){
	globals[idx] = val;	
	pop();
}

InterpretResult VM::run(){

#define READ_BYTE() (*ip++)
#define READ_BYTE_LONG() (READ_BYTE() << 16 | READ_BYTE() << 8 | READ_BYTE())
#define READ_CONSTANT() (chunk.constants[READ_BYTE()])
#define READ_CONSTANT_LONG() (chunk.constants[READ_BYTE() << 16 | \
											  READ_BYTE() << 8 | READ_BYTE()])

#define BINARY_OP(op) \
	do { \
	b = pop(); \
	a = pop(); \
	push((a op b)); \
	} while(false)

// init->
	init();
// Start Loop -->
	for(;;){
	#ifdef DEBUG_TRACE_EXECUTION
		fmt::print("\t");
		for(Value value : stack){
			fmt::print("[ {} ]", value);
		}
		fmt::println("");
		disassembleInstruction(chunk, std::distance(chunk.code.begin(), ip));
	#endif
		uint8_t instruction;
		Value a, b; // dummy variables to store values.
		int idx;
		switch(instruction = READ_BYTE()){
			case OP_ADD: BINARY_OP(+); break;
			case OP_SUB: BINARY_OP(-); break;
			case OP_MUL: BINARY_OP(*); break;
			case OP_DIV: BINARY_OP(/); break;
			case OP_POP: pop(); break;
			case OP_PRINT: fmt::println("{}", pop()); break;
			case OP_SIN: push(sin(pop())); break;
			case OP_COS: push(cos(pop())); break;
			case OP_TAN: push(tan(pop())); break;
			case OP_SQRT: push(sqrt(pop())); break;
			case OP_CONSTANT:
				a = READ_CONSTANT();
				push(a);
				break;
			case OP_CONSTANT_LONG:
				a = READ_CONSTANT_LONG();
				push(a);
				break;
			case OP_GET_GLOBAL:
				a = READ_CONSTANT();
				if(!getVariable(a)) return INTERPRET_RUNTIME_ERROR;
				break;
			case OP_GET_GLOBAL_LONG:
				a = READ_CONSTANT_LONG();
				if(!getVariable(a)) return INTERPRET_RUNTIME_ERROR;
				break;
			case OP_SET_GLOBAL:
				a = READ_CONSTANT();
				if(!setVariable(a, peek(0))) return INTERPRET_RUNTIME_ERROR;
				break;
			case OP_SET_GLOBAL_LONG:
				a = READ_CONSTANT_LONG();
				if(!setVariable(a, peek(0))) return INTERPRET_RUNTIME_ERROR;
				break;
			case OP_DEFINE_GLOBAL:
				a = READ_CONSTANT();
				defineVariable(a, peek(0));
				break;
			case OP_DEFINE_GLOBAL_LONG:
				a = READ_CONSTANT_LONG();
				defineVariable(a, peek(0));
				break;
			case OP_RETURN:
				return INTERPRET_OK;
		}
	}
// End Loop
#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}