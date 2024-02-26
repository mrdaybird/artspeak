#pragma once
#include <vector>
#include <map>

#include "Token.hpp"
#include "Runner.hpp"

namespace art{
class Scanner{
	public:
	Scanner(Runner& runner, std::string source);
	std::vector<Token> scanTokens();
	
	// Private variables and methods
	private:
	Runner& runner;
	std::string source;
	std::vector<Token> tokens;
	std::map<std::string, TokenType> keywords;
	int line = 1, col = 1, start = 0, current = 0;

	bool isAtEnd();
	void scanToken();
	char advance();
	void addToken(TokenType type);
	void addToken(TokenType type, std::string literal);
	void addToken(TokenType type, double literal);
	bool match(char c);
	char peek(int offset);
	void multiComment();
	void string();
	void number();
	void identifier();
};
}