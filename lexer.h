#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>

// Definitions move here so they are available to any file that #includes this
enum TokenType {KEYWORD, INDENT, NUMBER, MULTIPLY, DIVIDE,PLUS, MINUS, EQUALS, SEMICOLON, UNKNOWN };

struct Token {
    TokenType type;
    std::string value;
};

// The function signature
std::vector<Token> run_lexer(std::string filename);

#endif