#pragma once

#include "tokentype.hpp"
#include <string>

typedef struct {
	int line;
	TokenType type;
	std::string_view lexeme;
} Token;

class Scanner{
private:
	std::string_view source;
	size_t start, count;
	int line = 1;
private:
	void skipWhitespace();
	char advance();
	char peek(int offset = 0);
	bool isAtEnd();
	Token makeToken(TokenType type);
	bool match(char);
	Token string();
	Token number();
	TokenType checkKeyword(int start, int length, std::string_view expt, 
							TokenType want);
	TokenType identifierType();
	Token identifier();
	Token errorToken(std::string_view msg);
public:
	Scanner() = default;
	Scanner(std::string_view source);
	void init(std::string_view source);
	Token scanToken();	
};