#include "scanner.hpp"

Scanner::Scanner(std::string_view source) : source(source) {
	start = 0;
	count = 0;
	line = 1;
}
void Scanner::init(std::string_view source){
	this->source = source;
	start = 0;
	count = 0;
	line = 1;
}
char Scanner::advance(){
	count++;
	return source[start + count - 1];
}

Token Scanner::errorToken(std::string_view msg){
	Token tok = {
		.type = TOKEN_ERROR,
		.lexeme = msg,
	};
	return tok;
}

char Scanner::peek(int offset){
	if(start + count + offset >= source.length()) return '\0';
	return source[start + count + offset];
}

bool Scanner::isAtEnd(){
	if(start + count >= source.length()) return true;
	return false;
}

bool Scanner::match(char c){
	if(isAtEnd()) return false;
	if(peek() != c) return false;
	advance();
	return true; 
}

Token Scanner::makeToken(TokenType type){
	Token tok = {
		.line = line,
		.type = type,
		.lexeme = source.substr(start, count)
	};

	return tok;
}

void Scanner::skipWhitespace(){
	for(;;){
		switch(peek()){
			case ' ':
			case '\t':
			case '\276':
				advance();
				break;
			case '\n':
				advance();
				line++;
				break;
			case '/':
				if(peek(1) == '/'){
					while(!isAtEnd() && peek(0) != '\n') advance();
				} else return;
				break;
			default:
				return;
		}
	}
}

Token Scanner::number(){
	while(!isAtEnd() && isdigit(peek())) advance();
	if(peek() == '.' && isdigit(peek(1))){
		advance();
		while(!isAtEnd() && isdigit(peek())) advance();
	}

	return makeToken(TOKEN_NUMBER);
}

TokenType Scanner::checkKeyword(int pos, int length, std::string_view expt, TokenType want){
	if(count != (size_t)pos+length) 
		return TOKEN_IDENTIFIER;
	for(int i = 0; i < length; i++){
		if(expt[i] != source[start+pos+i]) return TOKEN_IDENTIFIER;
	}
	return want;
}

TokenType Scanner::identifierType(){
	switch(source[start]){
		case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
		case 'c': 
			if(count > 1){
				switch (source[start+1]) {
					case 'o': return checkKeyword(2, 1, "s", TOKEN_COS);
					case 'l': return checkKeyword(2, 3, "ass", TOKEN_CLASS);
				}
			}
		case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
		case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
		case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
		case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
		case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
		case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
		case 's': 
			if (count > 1){
				switch (source[start+1]) {
					case 'i': return checkKeyword(2, 1, "n", TOKEN_SIN);
					case 'u': return checkKeyword(2, 3, "per", TOKEN_SUPER);
				}
			}
		return checkKeyword(1, 4, "uper", TOKEN_SUPER);
		case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
		case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
		case 'f':
			if (count > 1) {
				switch (source[start+1]) {
					case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
					case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
					case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
				}
			}
			break;
		case 't':
			if (count > 1) {
				switch (source[start+1]) {
					case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
					case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
					case 'a': return checkKeyword(2, 1, "n", TOKEN_TAN);
				}
			}
			break;
	}

	return TOKEN_IDENTIFIER;
}

static bool isAlpha(char c){
	return (c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z')
			|| c == '_';
}

static bool isAlphaNumeric(char c){
	return isAlpha(c) || isdigit(c);
}

Token Scanner::identifier(){
	while(!isAtEnd() && isAlphaNumeric(peek())) advance();
	return makeToken(identifierType());
}

Token Scanner::string(){
	while(!isAtEnd() && peek() != '"'){
		if(peek() == '\n') line++;
		advance();
	}
	
	if(isAtEnd()) return errorToken("Unexpected EOF! Unterminated string!");

	advance();
	return makeToken(TOKEN_STRING);
}

Token Scanner::scanToken(){
	skipWhitespace();
	start = start + count;
	count = 0;

	if(isAtEnd()) return makeToken(TOKEN_EOF);

	char c = advance();

	// isdigit from the standard library
	if(isdigit(c)) return number();
	if(isAlpha(c)) return identifier();
	switch(c){
		case '(': return makeToken(TOKEN_LEFT_PAREN);
		case ')': return makeToken(TOKEN_RIGHT_PAREN);
		case '{': return makeToken(TOKEN_LEFT_BRACE);
		case '}': return makeToken(TOKEN_RIGHT_BRACE);
		case ';': return makeToken(TOKEN_SEMICOLON);
		case ',': return makeToken(TOKEN_COMMA);
		case '.': return makeToken(TOKEN_DOT);
		case '-': return makeToken(TOKEN_MINUS);
		case '+': return makeToken(TOKEN_PLUS);
		case '/': return makeToken(TOKEN_SLASH);
		case '*': return makeToken(TOKEN_STAR);
		case '!':
			return makeToken(
				match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=':
			return makeToken(
				match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<':
			return makeToken(
				match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>':
			return makeToken(
				match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		case '"': return string();
	}

	return errorToken("Unexpected Character!");
}

