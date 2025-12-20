#pragma once

#include <string>

enum TokenType {
    T_INT,
    T_DOUBLE,
    T_ID,
    T_KEYWORD,
    T_OPERATOR,
    T_CONST_INT,
    T_CONST_DOUBLE,
    T_DELIMITER,
    T_UNKNOWN,
    T_EOF
};

struct Token {
    TokenType type;
    std::string lex;
    Token() : type(T_UNKNOWN), lex("") {}
    Token(TokenType t, const std::string& l) : type(t), lex(l) {}
};


