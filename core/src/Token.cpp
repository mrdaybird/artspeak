#include "Token.hpp"
#include <cstring>

namespace art{
	Token::Token(TokenType type, std::string lexeme, int line, int col) :
		type(type), lexeme(std::move(lexeme)), line(line), col(col)
	{
		// Nothing to do here.
	}
	Token::Token(TokenType type, std::string lexeme, double literal, 
		int line, int col) : type(type), lexeme(std::move(lexeme)),
							 line(line), col(col)
	{			
		this->num_literal = literal;
	}
	Token::Token(TokenType type, std::string lexeme, std::string literal, 
		int line, int col) : type(type), lexeme(std::move(lexeme)),
							 line(line), col(col)
	{			
		this->str_literal = std::move(literal);
	}
	std::string Token::to_string() const{
		return "Type#" + std::to_string(type) + " " + lexeme + " " + 
			(type == STRING ? str_literal : 
				type == NUMBER ? std::to_string(num_literal) : " ");
	}
}