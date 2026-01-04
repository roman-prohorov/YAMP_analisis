#pragma once

#include "Token.h"
#include <fstream>
#include <string>

class Lexical {
    std::ifstream fin;
    int cursym;
    int line;
public:
    Lexical(const std::string& filename);
    ~Lexical();
    Token getNextTok();
private:
    void nextsym();
    bool isKeyword(const std::string& s);
};
