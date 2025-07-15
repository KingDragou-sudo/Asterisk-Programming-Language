#pragma once
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <string.h>

enum TokenType{
    //literal types
    IDENTIFIER, INT, STRING, FLOAT, CHAR, BOOL,

    //keywords
    IF, THEN, RET, WHILE, FOR, ELSE,
    CONTINUE, BREAK, IN, ROOM, VAR, FUNC,

    //arithmetic
    PLUS, MINUS, EQUALS, STAR, SLASH, CARET,

    //separators
    OPEN_PAREN, CLOSE_PAREN, OPEN_CURLY, CLOSE_CURLY, OPEN_BRACK, CLOSE_BRACK, COMMA, SEMICOLON, COLON,

    //function types
    PRINT, ROUND, FLOOR, CEIL, ABS, MIN, MAX, SQRT, POW,

    //array types
    ROOM_IDENTIFIER
};

struct Token{
    TokenType type;
    std::string value;

    Token(TokenType ttype, std::string str) : type(ttype), value(str) {}
};

std::vector<Token> lexer(std::string line){
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < line.size()) {
        if (isspace(line[i])) {
            i++;
            continue;
        }
        
        if (strchr("+-=*/()'{}[],;:^", line[i])) {
            switch (line[i]) {
                case '+': tokens.push_back(Token(PLUS, "+")); break;
                case '-': tokens.push_back(Token(MINUS, "-")); break;
                case '=': tokens.push_back(Token(EQUALS, "=")); break;
                case '*': tokens.push_back(Token(STAR, "*")); break;
                case '/': tokens.push_back(Token(SLASH, "/")); break;
                case '(': tokens.push_back(Token(OPEN_PAREN, "(")); break;
                case ')': tokens.push_back(Token(CLOSE_PAREN, ")")); break;
                case '[': tokens.push_back(Token(OPEN_BRACK, "[")); break;
                case ']': tokens.push_back(Token(CLOSE_BRACK, "]")); break;
                case '{': tokens.push_back(Token(OPEN_CURLY, "{")); break;
                case '}': tokens.push_back(Token(CLOSE_CURLY, "}")); break;
                case ',': tokens.push_back(Token(COMMA, ",")); break;
                case ';': tokens.push_back(Token(SEMICOLON, ";")); break;
                case ':': tokens.push_back(Token(COLON, ":")); break;
                case '^': tokens.push_back(Token(CARET, "^")); break;
            }
            ++i;
            continue;
        }

        if (line[i] == '"') {
            size_t start = i++;
            while (i < line.size() && line[i] != '"') ++i;
            if (i < line.size()) ++i;
            tokens.push_back(Token(STRING, line.substr(start, i - start)));
            continue;
        }
        
        if (line[i] == '\'') {
            size_t start = i++;
            while (i < line.size() && line[i] != '\'') ++i;
            if (i < line.size()) ++i;
            tokens.push_back(Token(CHAR, line.substr(start, i - start)));
            continue;
        }
        
        if (isdigit(line[i])) {
            size_t start = i;
            bool hasDot = false;
            while (i < line.size() && (isdigit(line[i]) || (!hasDot && line[i] == '.'))) {
                if (line[i] == '.') hasDot = true;
                ++i;
            }
            bool isFloat = hasDot;
            if (i < line.size() && line[i] == 'f') {
                ++i;
                isFloat = true;
            }
            if (isFloat) tokens.push_back(Token(FLOAT, line.substr(start, i - start)));
            else tokens.push_back(Token(INT, line.substr(start, i - start)));
            continue;
        }
        
        if (isalpha(line[i]) || line[i] == '_') {
            size_t start = i;
            while (i < line.size() && (isalnum(line[i]) || line[i] == '_')) ++i;
            std::string word = line.substr(start, i - start);
            
            if (word.length() > 5 && word.substr(word.length() - 5) == "_ROOM") {
                tokens.push_back(Token(ROOM_IDENTIFIER, word));
            }
            else if(word == "if") tokens.push_back(Token(IF, word));
            else if(word == "then") tokens.push_back(Token(THEN, word));
            else if(word == "ret") tokens.push_back(Token(RET, word));
            else if(word == "for") tokens.push_back(Token(FOR, word));
            else if(word == "else") tokens.push_back(Token(ELSE, word));
            else if(word == "continue") tokens.push_back(Token(CONTINUE, word));
            else if(word == "break") tokens.push_back(Token(BREAK, word));
            else if(word == "in") tokens.push_back(Token(IN, word));
            else if(word == "room") tokens.push_back(Token(ROOM, word));
            else if(word == "var") tokens.push_back(Token(VAR, word));
            else if(word == "func") tokens.push_back(Token(FUNC, word));
            else if(word == "true" || word == "false") tokens.push_back(Token(BOOL, word));
            else if(word == "print") tokens.push_back(Token(PRINT, word));
            else if(word == "round") tokens.push_back(Token(ROUND, word));
            else if(word == "floor") tokens.push_back(Token(FLOOR, word));
            else if(word == "ceil") tokens.push_back(Token(CEIL, word));
            else if(word == "abs") tokens.push_back(Token(ABS, word));
            else if(word == "min") tokens.push_back(Token(MIN, word));
            else if(word == "max") tokens.push_back(Token(MAX, word));
            else if(word == "sqrt") tokens.push_back(Token(SQRT, word));
            else if(word == "pow") tokens.push_back(Token(POW, word));
            else tokens.push_back(Token(IDENTIFIER, word));
            continue;
        }

        ++i;
    }

    return tokens;
}
