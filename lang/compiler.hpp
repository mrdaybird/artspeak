#pragma once

// Job of the compiler is to take some string and convert into bytecode for the VM to read.
// It maintains a instance of ->
// 1. Scanner which tokenizes the string
// 2. Parser that converts bunch of tokens and fills up Chunk with instructions
// Compiler class encapsulates the Parser and Scanner logic

#include "scanner.hpp"
#include "chunk.hpp"
#include <string>
#include <memory>

typedef struct {
	Token previous, current;
	bool panicMode, hadError; // I don't know the need for hadError? 
} Parser;

/*
	declaration -> varDecl 
				| statement ;
	
	varDecl -> "var" identifier ("=" expression) ";"
	statement -> printStmt 
				| exprStmt; 
	
	printStmt -> "print" expression ";" ;
	exprStmt  -> expression ";" ;
*/

enum Precedence{
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
  	PREC_AND,         // and
	PREC_EQUALITY,    // == !=
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_UNARY,       // ! -
	PREC_CALL,        // . ()
	PREC_PRIMARY
};


class Compiler{
public:
	Scanner sc;
	Parser parser;
	Chunk &compilingChunk;
public:
	Chunk& currentChunk();
	void endCompiler();

	void errorAt(Token &token, std::string_view msg);
	void errorAtCurrent(std::string_view msg);
	void error(std::string_view msg);

	// Block: Parsing token helpers ->
	void advance();
	bool check(TokenType type);
	bool match(TokenType type);
	void consume(TokenType type, std::string_view msg);
	// End Block

	// Block: (Byte)CodeGen helpers
	void emitByte(uint8_t byte);
	void emitBytes(uint8_t b1, uint8_t b2);
	void emitConstant(Value constant);
	void emitOpWithConstant(uint8_t smallOp, uint8_t longOp, int idx);
	// void emitReturn();
	// End Block

	// Block: Grammar helpers ->
	void 	parsePrecedence(Precedence prec);
	void 	expression();
	void 	printStmt();
	void 	exprStmt();
	int 	makeConstant(Value constant);
	int 	identifierConstant(Token &token);
	int 	parseVariable(std::string_view msg);
	void 	defineVariable(int idx);
	void 	varDecl();
	void 	statement();
	void 	declaration();
	// End Block

public:
	Compiler(Chunk &chunk) : compilingChunk(chunk) {

	}
	~Compiler() = default;
	bool compile(std::string_view source, bool canvasMode);
};