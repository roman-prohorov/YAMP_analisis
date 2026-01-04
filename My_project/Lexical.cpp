#include "Lexical.h"
#include <cctype>

Lexical::Lexical(const std::string& filename) : fin(filename), line(1) {
    cursym = fin.get();
}

Lexical::~Lexical() {
    if (fin.is_open()) 
        fin.close();
}

void Lexical::nextsym() {
    cursym = fin.get();
    if (cursym == '\n') 
        line++;
}

bool Lexical::isKeyword(const std::string& s) {
    static const std::string keywords[] = {
        "int", "double", "return"
    };
    for (const auto& k : keywords)
        if (k == s) return true;
    return false;
}

Token Lexical::getNextTok() {
    enum Sost { 
        INT_CONST,
        DOUBLE_CONST
        START, 
        ID, 
        DOT, 
        DONE, 
        ERROR,
    } 
    sost = START;
    std::string lexeme;
    TokenType type = T_UNKNOWN;
    while (true) {
        switch (sost) {
        case START:
            if (cursym == EOF || fin.eof()) return Token(T_EOF, "");
            else if (isspace(cursym)) nextsym();
            else if (isdigit(cursym)) {
                lexeme += (char)cursym;
                sost = INT_CONST;
                nextsym();
            }
            else if (cursym == '.') {
                lexeme += '.';
                sost = DOT;
                nextsym();
            }
            else if (isalpha(cursym)) {
                lexeme += (char)cursym;
                sost = ID;
                nextsym();
            }
            else if (cursym == ';' || cursym == ',' || cursym == '(' || cursym == ')' || cursym == '{' || cursym == '}' || cursym == '+' || cursym == '-' || cursym == '*' || cursym == '/' || cursym == '=') {
                lexeme += (char)cursym;
                type = T_OPERATOR;
                if (cursym == ';' || cursym == ',' || cursym == '(' || cursym == ')' || cursym == '{' || cursym == '}') {
                    type = T_DELIMITER;
                }
                nextsym();
                return Token(type, lexeme);
            }
            else {
                lexeme += (char)cursym;
                nextsym();
                return Token(T_UNKNOWN, lexeme);
            }
            break;
        case ID:
            if (isalnum(cursym)) {
                if (isdigit(cursym) && !isalpha(lexeme.back())) {
                    lexeme += (char)cursym;
                }
                else if (isalpha(lexeme.back()) || isdigit(cursym)) {
                    lexeme += (char)cursym;
                }
                else {
                    sost = ERROR;
                    lexeme += (char)cursym;
                }
                nextsym();
            }
            else {
                type = (isKeyword(lexeme) ? T_KEYWORD : T_ID);
                return Token(type, lexeme);
            }
            break;
        case INT_CONST:
            if (isdigit(cursym)) {
                if (lexeme == "0") {
                    sost = ERROR;
                    lexeme += (char)cursym;
                    nextsym();
                }
                else {
                    lexeme += (char)cursym;
                    nextsym();
                }
            }
            else if (cursym == '.') {
                lexeme += (char)cursym;
                sost = DOUBLE_CONST;
                nextsym();
            }
            else {
                return Token(T_CONST_INT, lexeme);
            }
            break;
        case DOUBLE_CONST:
            if (isdigit(cursym)) { 
                lexeme += (char)cursym;
                nextsym();
            }
            else {
                return Token(T_CONST_DOUBLE, lexeme);
            }
            break;

        case DOT: 
            return Token(T_UNKNOWN, lexeme);

        case ERROR:
            return Token(T_UNKNOWN, lexeme);

        default:
            return Token(T_UNKNOWN, lexeme);
        }
    }
}

