#pragma once

#include "Token.h"
#include "Lexical.h"
#include <fstream>
#include <vector>

struct ParseTreeNode {
    std::string name; 
    std::vector<ParseTreeNode*> children;
    Token token;
    ParseTreeNode(const std::string& n) : name(n) {}
    ParseTreeNode(const std::string& n, const Token& t) : name(n), token(t) {} 
    ~ParseTreeNode();
};

class Parsing {
public:
    bool parsing();
    Parsing(Lexical& l, std::ofstream& o);
    ~Parsing();
    Lexical& lex;
    Token tekT;
    std::ofstream& out;
    bool erNow;
    int cnter;
    int pol;
    bool prer;
    void advance();
    void match(TokenType vsT, const std::string& vsL = "");
    void error(const std::string& input, int line);
    bool ser(int line);
    void proc(TokenType vsT, const std::string& vsL = "");
    void skip(TokenType type, const std::string& lex = "");
    int tz();
    int unk();
    ParseTreeNode* parseFunction();
    ParseTreeNode* parseBegin();
    ParseTreeNode* parseEnd();
    ParseTreeNode* parseFunctionName();
    ParseTreeNode* parseDescriptions();
    ParseTreeNode* parseOperators();
    ParseTreeNode* parseDescr();
    ParseTreeNode* parseVarList();
    ParseTreeNode* parseType();
    ParseTreeNode* parseOp();
    ParseTreeNode* parseSimpleExpr();
    ParseTreeNode* parseExpr();
    ParseTreeNode* parseTerm();
    ParseTreeNode* parseId();
    ParseTreeNode* parseConst();
    void print(ParseTreeNode* node, int depth = 0);
};