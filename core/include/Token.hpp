#pragma once 

#include <string>
namespace art{
	enum TokenType{
		// Single-character tokens.
		LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
		COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
		//Extras
		COLON, QUES_MARK,//Ternary Expression tokens ?:

		// One or two character tokens.
		BANG, BANG_EQUAL,
		EQUAL, EQUAL_EQUAL,
		GREATER, GREATER_EQUAL,
		LESS, LESS_EQUAL,

		// Literals.
		IDENTIFIER, STRING, NUMBER,

		// Keywords.
		AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
		PRINT, CONTINUE, BREAK, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

		ART_EOF
	};
	class Token{
		public:
		Token(TokenType type, std::string, int line, int col);
		Token(TokenType type, std::string lexeme, double literal,
			 int line, int col);
		Token(TokenType type, std::string lexeme, std::string literal,
			 int line, int col);
		std::string to_string() const;
		
		// Private methods and variables
		private:
		std::string str_literal;
		double num_literal;
		TokenType type;
		std::string lexeme;
		int line, col;

	};
}