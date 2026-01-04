#pragma once
#include "Syntaxich.h"
#include "HashTable.h"
#include <fstream>
#include <string>
#include <vector>

enum DataType {
    DT_INT = 200,     
    DT_DOUBLE = 201,
    DT_ERROR = 202,
    DT_UNKNOWN = 203
};

class Semantic {
public:
    Semantic(std::ofstream& o);
    ~Semantic();
    bool analyzis(ParseTreeNode* root);
    std::ofstream& out;
    HashTable symbolTable;
    DataType tf;
    void notIsppr();
    void error(const std::string& input);
    std::string dataStr(DataType type);
    DataType TtoD(TokenType type);
    TokenType DtoT(DataType type);
    std::string post;
    int cnter;
    int tekstr;
    bool ispr(const std::string& Name);
    DataType getType(const std::string& Name);
    DataType rExpr(ParseTreeNode* node);
    DataType rTerm(ParseTreeNode* node);
    DataType rSimpleExpr(ParseTreeNode* node);
    void pr(const std::string& Name, DataType type);
    void isppr(const std::string& Name);
    void rBegin(ParseTreeNode* node);
    void rDescriptions(ParseTreeNode* node);
    void rOperators(ParseTreeNode* node);
    void rEnd(ParseTreeNode* node);
    void rDescr(ParseTreeNode* node);
    void rOp(ParseTreeNode* node);
};