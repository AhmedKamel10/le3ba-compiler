#include "lexer.h"
#include <iostream>
#include <fstream>

using namespace std;

vector<Token> run_lexer(string filename) {
    ifstream inputFile(filename);
    vector<Token> tokens;
    string thisToken;

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open " << filename << endl;
        return tokens;
    }

    char prev = 0;

    string line;
    while (getline(inputFile, line)) {
        for (char c : line) {
            if (isspace(c)) {
                if (!thisToken.empty()) {
                    TokenType t;
                    if (isdigit(thisToken[0])) t = NUMBER;
                    else if (thisToken == "int" || thisToken == "string" || thisToken == "float" || thisToken == "if")
                        t = KEYWORD;
                    else t = INDENT;

                    tokens.push_back({t, thisToken});
                    thisToken.clear();
                }
            }
            else if (ispunct(c)) {

                if (!thisToken.empty()) {
                    TokenType t = isdigit(thisToken[0]) ? NUMBER : INDENT;
                    tokens.push_back({t, thisToken});
                    thisToken.clear();
                }

                // handle ==
                if (c == '=' && prev == '=') {
                    tokens.back().value = "==";
                    prev = 0;
                    continue;
                }

                TokenType t = UNKNOWN;
                if (c == '=') t = EQUALS;
                else if (c == ';') t = SEMICOLON;
                else if (c == '+') t = PLUS;
                else if (c == '-') t = MINUS;
                else if (c == '*') t = MULTIPLY;
                else if (c == '/') t = DIVIDE;

                tokens.push_back({t, string(1, c)});
                prev = c;
            }
            else {
                thisToken += c;
                prev = 0;
            }
        }
    }

    if (!thisToken.empty()) {
        TokenType t = isdigit(thisToken[0]) ? NUMBER : INDENT;
        tokens.push_back({t, thisToken});
    }

    inputFile.close();
    return tokens;
}