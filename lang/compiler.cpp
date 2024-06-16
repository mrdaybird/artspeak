#include "compiler.hpp"
#include "debug.hpp"
#include <fmt/core.h>
#include <stdio.h>

void Compiler::errorAt(Token &token, std::string_view msg){
	if(parser.panicMode) return;
	parser.panicMode = true;

	fmt::print(stderr, "[line {}] Error", token.line);
	if(token.type == TOKEN_EOF)
		fmt::print(stderr, " at the end.");
	else if(token.type != TOKEN_ERROR)
		fmt::print(stderr, " at {}.", token.lexeme);
	
	fmt::println(": {}", msg);
	parser.hadError = true;
}

void Compiler::errorAtCurrent(std::string_view msg){
	errorAt(parser.current, msg);
}

void Compiler::error(std::string_view msg){
	errorAt(parser.previous, msg);
}

void Compiler::advance(){
	parser.previous = parser.current;

	// loop because if it is a problematic token we still parse the rest
	// of the tokens.
	for(;;){
		parser.current = sc.scanToken();
		if(!check(TOKEN_ERROR)) break;
	
		errorAtCurrent(parser.current.lexeme);
	}
}

bool Compiler::check(TokenType type){
	return parser.current.type == type;
}

bool Compiler::match(TokenType type){
	if(!check(type)) return false;
	advance();
	return true;
}

void Compiler::consume(TokenType type, std::string_view msg){
	if(check(type)){
		advance();
		return;
	}

	errorAtCurrent(msg);
}

int Compiler::makeConstant(Value constant){
	int idx = currentChunk().addConstant(constant);
	if(idx >= STACK_MAX){
		error("Too many constants");
		return 0;
	}
	return idx;
}

int Compiler::identifierConstant(Token &token){
	auto &globals = currentChunk().globals;
	int idx;
	std::string varName{token.lexeme};
	if(globals.find(varName) != globals.end()){
		idx = globals[varName];
	}else{
		idx = globals.size();
		globals[varName] = idx;
	}
	return makeConstant(idx);	
	// return idx;
}

Chunk& Compiler::currentChunk(){
	return compilingChunk;
}

void Compiler::emitByte(uint8_t byte){
	currentChunk().write(byte, parser.previous.line);
}

void Compiler::emitBytes(uint8_t b1, uint8_t b2){
	emitByte(b1);
	emitByte(b2);
}

void Compiler::emitConstant(Value constant){
	if(currentChunk().writeConstantOp(constant, parser.previous.line) == -1){
		error("Too many constant!");
	}
}

void Compiler::emitOpWithConstant(uint8_t smallOp, uint8_t longOp, 
	int idx){
	if(idx < 256){
		emitByte(smallOp);
		emitByte(idx);
	}else{
		emitBytes(longOp, (uint8_t)idx >> 16);
		emitBytes((uint8_t)idx >> 8, (uint8_t)idx);
	}
}

// Block: Parsing Logic:

typedef void (*ParseFn)(Compiler &c, bool canAssign);

enum ParseType{
	PARSE_NULL = 0,
	PARSE_CALL,
	PARSE_STRING,
	PARSE_LITERAL,
	PARSE_NUMBER,
	PARSE_GROUPING,
	PARSE_UNARY,
	PARSE_BINARY,
	PARSE_VARIABLE
};

typedef struct{
	ParseType prefix;
	ParseType infix;
	Precedence precedence;
} ParseRule;

static ParseRule getRule(TokenType type);

static void string(Compiler &c, bool canAssign){
	c.error("Strings not supported!");
}

static void number(Compiler &c, bool canAssign){
	double constant = strtod(std::string(c.parser.previous.lexeme).c_str(), NULL);
	c.emitConstant(constant);
}

static void literal(Compiler &c, bool canAssign){
	c.error("Literal not supported!");
}

static void grouping(Compiler &c, bool canAssign){
	c.expression();
	c.consume(TOKEN_RIGHT_PAREN, "Parenthesis not closed!");
}

static void unary(Compiler &c, bool canAssign){
	TokenType opType = c.parser.previous.type;
	c.parsePrecedence(PREC_UNARY);

	switch(opType){
		case TOKEN_MINUS: c.emitByte(OP_NEGATE); break;
		default: c.error("Not supported!");
	}
}

static void binary(Compiler &c, bool canAssign){
	TokenType opType = c.parser.previous.type;
	ParseRule rule = getRule(opType);
	c.parsePrecedence((Precedence)(rule.precedence + 1));

	switch(opType){
		case TOKEN_PLUS: 			c.emitByte(OP_ADD); break;
		case TOKEN_MINUS: 			c.emitByte(OP_SUB); break;
		case TOKEN_STAR: 			c.emitByte(OP_MUL); break;
		case TOKEN_SLASH: 			c.emitByte(OP_DIV); break;
		case TOKEN_BANG_EQUAL:		c.emitBytes(OP_EQUAL, OP_NOT); break;
		case TOKEN_EQUAL_EQUAL: 	c.emitByte(OP_EQUAL); break;
		case TOKEN_GREATER: 		c.emitByte(OP_GREATER); break;
		case TOKEN_GREATER_EQUAL: 	c.emitBytes(OP_GREATER, OP_NOT); break;
		case TOKEN_LESS: 			c.emitByte(OP_LESS); break;
		case TOKEN_LESS_EQUAL: 		c.emitBytes(OP_LESS, OP_NOT); break;
		default: break;
	}
}

static void namedVariable(Compiler &c, Token &token, bool canAssign){
	int arg = c.identifierConstant(token);
	if(canAssign && c.match(TOKEN_EQUAL)){
		c.expression();
		c.emitOpWithConstant(OP_SET_GLOBAL, OP_SET_GLOBAL_LONG, arg);
	}else{
		c.emitOpWithConstant(OP_GET_GLOBAL, OP_GET_GLOBAL_LONG, arg);
	}
}

static void variable(Compiler &c, bool canAssign){
	namedVariable(c, c.parser.previous, canAssign);
}

static void funCall(Compiler &c, bool canAssign){
	TokenType funType = c.parser.previous.type;
	c.consume(TOKEN_LEFT_PAREN,	 "Missing left parenthesis!");
	c.expression();
	c.consume(TOKEN_RIGHT_PAREN, "Missing right parenthesis!");
	switch(funType){
		case TOKEN_SIN: c.emitByte(OP_SIN); break;
		case TOKEN_COS: c.emitByte(OP_COS); break;
		case TOKEN_TAN: c.emitByte(OP_TAN); break;
		default: c.error("Unknown function!");
	}
}

static void parse(ParseType type, Compiler &c, bool canAssign){
	switch(type){
		case PARSE_GROUPING: grouping(c, canAssign); break;
		case PARSE_UNARY: unary(c, canAssign); break;
		case PARSE_BINARY: binary(c, canAssign); break;
		case PARSE_LITERAL: literal(c, canAssign); break;
		case PARSE_NUMBER: number(c, canAssign); break; 
		case PARSE_STRING: string(c, canAssign); break;
		case PARSE_VARIABLE: variable(c, canAssign); break;
		case PARSE_CALL: funCall(c, canAssign); break;
		default: break;
	}
}


static ParseRule getRule(TokenType type){
  switch(type){
	case TOKEN_LEFT_PAREN    : return  {PARSE_GROUPING, PARSE_NULL,   PREC_NONE};
	case TOKEN_RIGHT_PAREN   : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_LEFT_BRACE    : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_RIGHT_BRACE   : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_COMMA         : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_DOT           : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_MINUS         : return  {PARSE_UNARY,    PARSE_BINARY, PREC_TERM};
	case TOKEN_PLUS          : return  {PARSE_NULL,     PARSE_BINARY, PREC_TERM};
	case TOKEN_SEMICOLON     : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_SLASH         : return  {PARSE_NULL,     PARSE_BINARY, PREC_FACTOR};
	case TOKEN_STAR          : return  {PARSE_NULL,     PARSE_BINARY, PREC_FACTOR};
	case TOKEN_BANG          : return  {PARSE_UNARY,    PARSE_NULL,   PREC_NONE};
	case TOKEN_EQUAL         : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_BANG_EQUAL    : return  {PARSE_NULL,     PARSE_BINARY,   PREC_EQUALITY};
	case TOKEN_EQUAL_EQUAL   : return  {PARSE_NULL,     PARSE_BINARY,   PREC_COMPARISON};
	case TOKEN_GREATER       : return  {PARSE_NULL,     PARSE_BINARY,   PREC_COMPARISON};
	case TOKEN_GREATER_EQUAL : return  {PARSE_NULL,     PARSE_BINARY,   PREC_COMPARISON};
	case TOKEN_LESS          : return  {PARSE_NULL,     PARSE_BINARY,   PREC_COMPARISON};
	case TOKEN_LESS_EQUAL    : return  {PARSE_NULL,     PARSE_BINARY,   PREC_COMPARISON};
	case TOKEN_IDENTIFIER    : return  {PARSE_VARIABLE,     PARSE_NULL,   PREC_NONE};
	case TOKEN_STRING        : return  {PARSE_VARIABLE,     PARSE_NULL,   PREC_NONE};
	case TOKEN_NUMBER        : return  {PARSE_NUMBER,   PARSE_NULL,   PREC_NONE};
	case TOKEN_AND           : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_CLASS         : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_ELSE          : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_FALSE         : return  {PARSE_LITERAL,  PARSE_NULL,   PREC_NONE};
	case TOKEN_TRUE          : return  {PARSE_LITERAL,  PARSE_NULL,   PREC_NONE};
	case TOKEN_NIL           : return  {PARSE_LITERAL,  PARSE_NULL,   PREC_NONE};
	case TOKEN_FOR           : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_FUN           : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_IF            : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_OR            : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_PRINT         : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};	
	case TOKEN_SIN         	 : return  {PARSE_CALL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_COS         	 : return  {PARSE_CALL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_TAN         	 : return  {PARSE_CALL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_RETURN        : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_SUPER         : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_THIS          : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_VAR           : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_WHILE         : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_ERROR         : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	case TOKEN_EOF           : return  {PARSE_NULL,     PARSE_NULL,   PREC_NONE};
	default: break;
  } 
  return {PARSE_NULL, PARSE_NULL, PREC_NONE};
}

void Compiler::parsePrecedence(Precedence prec){
	advance();
	ParseType prefix = getRule(parser.previous.type).prefix;
	if(prefix == PARSE_NULL){
		error("Expected Expression!");
		return;
	}

	bool canAssign = prec <= PREC_ASSIGNMENT;
	parse(prefix, *this, canAssign);

	while(prec <= getRule(parser.current.type).precedence){
		advance();
		ParseType infix = getRule(parser.previous.type).infix;
		if(infix == PARSE_NULL){
			error("Incorrect Syntax?");
		}
		else parse(infix, *this, canAssign);
	}
	
	if(canAssign && match(TOKEN_EQUAL)){
		error("Invalid assignment target!");
	}
}

void Compiler::expression(){
	parsePrecedence(PREC_ASSIGNMENT);
}

void Compiler::printStmt(){
	expression();
	consume(TOKEN_SEMICOLON, "Expected ';' at the end of statement.");
	emitByte(OP_PRINT);
}

void Compiler::exprStmt(){
	expression();
	consume(TOKEN_SEMICOLON, "Expected ';' at the end of statement.");
	emitByte(OP_POP);
}

void Compiler::statement(){
	if(match(TOKEN_PRINT)){
		printStmt();
	}else{
		exprStmt();
	}
}

int Compiler::parseVariable(std::string_view msg){
	consume(TOKEN_IDENTIFIER, msg);
	return identifierConstant(parser.previous);
}

void Compiler::defineVariable(int idx){
	emitOpWithConstant(OP_DEFINE_GLOBAL, OP_DEFINE_GLOBAL_LONG, idx);
}

void Compiler::varDecl(){
	int global = parseVariable("Expected variable name!");

	if(match(TOKEN_EQUAL)){
		expression();
	}else{
		emitConstant(0);
	}

	consume(TOKEN_SEMICOLON, "Expected ';' after var declaration.");

	defineVariable(global);
}

void Compiler::declaration(){
	if(match(TOKEN_VAR)){
		varDecl();
	}else{
		statement();
	}
}

void Compiler::endCompiler(){
	emitByte(OP_RETURN);
#ifdef DEBUG_PRINT_CODE
	if(!parser.hadError){
		disassembleChunk(currentChunk(), "start diss!");
	}
#endif	
}

bool Compiler::compile(std::string_view source, bool canvasMode){
	sc.init(source);
	// compilingChunk = chunk;
	parser.hadError = false;
	parser.panicMode = false;

	advance();

	if(!canvasMode){
		while(!match(TOKEN_EOF)){
			declaration();
		}
	} else {
		expression();
	}
	

	endCompiler();
	return !parser.hadError;
}