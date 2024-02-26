#include "Scanner.hpp"

namespace art{
Scanner::Scanner(Runner& runner, std::string source) : 
	runner(runner), source(std::move(source)) 
{
	keywords.insert({
		{"and", AND},
		{"or", OR},
		{"while", WHILE},
		{"for", FOR},
		{"fun", FUN},
		{"class", CLASS},
		{"super", SUPER},
		{"this", THIS},
		{"if", IF},
		{"else", ELSE},
		{"true", TRUE},
		{"false", FALSE},
		{"continue", CONTINUE},
		{"break", BREAK},
		{"return", RETURN},
		{"nil", NIL},
		{"var", VAR},
		{"print", PRINT}
	});
}
std::vector<Token> Scanner::scanTokens(){
	while(!isAtEnd()){
		start = current;
		scanToken();
	}
	tokens.push_back(Token(ART_EOF, "", line, col));
	return tokens;
}
bool Scanner::isAtEnd(){
	return current >= source.length();
}
void Scanner::scanToken(){
	char c = advance();
	switch (c)
	{
		// single character tokens
		case '(': addToken(LEFT_PAREN); break;
		case ')': addToken(RIGHT_PAREN); break;
		case '{': addToken(LEFT_BRACE); break;
		case '}': addToken(RIGHT_BRACE); break;
		case '+': addToken(PLUS); break;
		case '-': addToken(MINUS); break;
		case '*': addToken(STAR); break;
		case ',': addToken(COMMA); break;
		case '.': addToken(DOT); break;
		case '?': addToken(QUES_MARK); break;
		case ':': addToken(COLON); break;
		case ';': addToken(SEMICOLON); break;

		// two character tokens
		case '/':
			if(match('/'))
				while(source[current] != '\n' && !isAtEnd()) advance();
			else if(match('*'))
				multiComment();
			else
				addToken(SLASH);
			break;
		case '!':
			if(match('=')) addToken(BANG_EQUAL);
			else addToken(BANG);
			break;
		case '=':
			if(match('=')) addToken(EQUAL_EQUAL);
			else addToken(EQUAL);
			break;
		case '<':
			if(match('=')) addToken(LESS_EQUAL);
			else addToken(LESS);
			break;
		case '>':
			if(match('=')) addToken(GREATER_EQUAL);
			else addToken(GREATER);
			break;
		case ' ':
		case '\r':
		case '\t':
			break;
		case '\n':
			col = 1;
			line++;
			break;
		case '"': string(); break;
		default:
			if(isdigit(c))
				number();
			else if(isalpha(c))
				identifier();
			else
				runner.error(line, col, "Unexpected character.");
	}
}
char Scanner::advance(){
	col++;
	return source[current++];
}
void Scanner::addToken(TokenType type){
	tokens.push_back(Token(type, source.substr(start, current-start), line, col));
}
void Scanner::addToken(TokenType type, std::string literal){
	tokens.push_back(Token(type, source.substr(start, current-start), std::move(literal), line, col));
}
void Scanner::addToken(TokenType type, double literal){
	tokens.push_back(Token(type, source.substr(start, current-start), literal, line, col));
}
bool Scanner::match(char c){
	if(!isAtEnd() && source[current] == c){
		advance();
		return true;	
	} 
	return false;
}
char Scanner::peek(int offset){
	if(current + offset >= source.length()) return '\0';
	return source[current + offset];
}
void Scanner::multiComment(){
	int toClose = 1;
	while(toClose && !isAtEnd()){
		if(peek(0) == '*' && peek(1) == '/') { toClose--; advance(); }
		if(peek(0) == '/' && peek(1) == '*') { toClose++; advance(); }
		advance();
	}
	if(toClose != 0 && isAtEnd()){
		runner.error(line, col, "Unterminated multi-line comments.");
	}
}
void Scanner::string(){
	while(peek(0) != '"' && !isAtEnd()){
		if(peek(0) == '\n') line++;
		advance();
	}
	if(isAtEnd()){
		runner.error(line, col, "Unterminated string.");
	}
	advance();
	std::string value = source.substr(start+1, current-start-2);
	addToken(STRING, value);
}
void Scanner::number(){
	while(isdigit(peek(0)) && !isAtEnd()) advance();
	if(peek(0) == '.' && isdigit(peek(1))){
		advance();
		while(isdigit(peek(0)) && !isAtEnd()) advance();
	}
	std::string lexeme = source.substr(start, current-start);
	double literal = atof(lexeme.c_str());
	addToken(NUMBER, literal);
}	
void Scanner::identifier(){
	while(isalnum(peek(0)) && !isAtEnd()) advance();
	std::string text = source.substr(start, current-start);
	TokenType type;
	if(keywords.find(text) != keywords.end()){
		type = keywords[text];
	}else{
		type = IDENTIFIER;
	}
	addToken(type);
}
}