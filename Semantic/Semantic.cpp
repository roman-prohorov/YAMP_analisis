#include "Semantic.h"
#include <iostream>
#include <sstream>

Semantic::Semantic(std::ofstream& o)
    : out(o), symbolTable(211), cnter(0), tf(DT_UNKNOWN), tekstr(1) {
}

Semantic::~Semantic() {}

void Semantic::error(const std::string& input) {
    out << "Semantic Error: " << input << std::endl;
    cnter++;
}

std::string Semantic::dataStr(DataType type) {
    switch (type) {
    case DT_UNKNOWN:
        return "unknown";
    case DT_ERROR:
        return "error";
    case DT_INT:
        return "int";
    case DT_DOUBLE:
        return "double";
    default:
        return "invalid";
    }
}

DataType Semantic:: TtoD(TokenType type) {
    switch (type) {
    case T_INT:
        return DT_INT;
    case T_DOUBLE:
        return DT_DOUBLE;
    default:
        return DT_UNKNOWN;
    }
}

TokenType Semantic::DtoT(DataType type) {
    switch (type) {
    case DT_INT:
        return T_INT;
    case DT_DOUBLE:
        return T_DOUBLE;
    default:
        return T_UNKNOWN;
    }
}
bool Semantic::ispr(const std::string& varName) {
    return symbolTable.contains(varName);
}
void Semantic::isppr(const std::string& varName) {
    symbolTable.marka(varName);
}

void Semantic::notIsppr() {
    std::vector<Symin*> symbols = symbolTable.allin();
    for (Symin* symbol : symbols) {
        if (symbol->token.type == T_ID && !symbol->isp)
            return;
    }
}
DataType Semantic::getType(const std::string& Name) {
    int typetek = symbolTable.getDatat(Name);
    switch (typetek) {
    case 0: 
        return DT_INT;
    case 1: 
        return DT_DOUBLE;
    case -1:
        return DT_UNKNOWN;
    default: 
        return DT_ERROR;
    }
}

void Semantic::pr(const std::string& Name, DataType type) {
    Token varToken(T_ID, Name);
    int typetek = -1;
    switch (type) {
    case DT_INT: 
        typetek = 0; 
        break;
    case DT_DOUBLE: 
        typetek = 1; 
        break;
    default:
        typetek = -1;
        break;
    }
    if (symbolTable.contains(Name)) {
        error("Variable redeclaration '" + Name + "' ");
        return;
    }
    int inx = symbolTable.insert(varToken);
    if (inx != -1) symbolTable.setDatat(Name, typetek);
}

bool Semantic::analyzis(ParseTreeNode* root) {
    symbolTable.clear();
    cnter = 0;
    post = "";
    tf = DT_UNKNOWN;
    if (root->children.size() < 4 || root->name != "Function") {
        error("Invalid parse tree");
        return false;
    }
    rBegin(root->children[0]);
    rDescriptions(root->children[1]);
    rOperators(root->children[2]);
    rEnd(root->children[3]);
    notIsppr();
    out << std::endl << "----POSTFIX----" << std::endl;
    out << post << std::endl;
    out << "Semantic analysis " << (cnter == 0 ? "completed successfully" : "failed") << ". Errors: " << cnter << std::endl;
    return (cnter == 0);
}

void Semantic::rBegin(ParseTreeNode* node) {
    if (node->children.size() < 2 || !node) {
        error("Incorrect function header");
        return;
    }
    ParseTreeNode* typeNode = node->children[0];
    ParseTreeNode* nameNode = node->children[1];
    if (!typeNode->children.empty()) {
        Token typeToken = typeNode->children[0]->token;
        if (typeToken.type == T_KEYWORD) {
            if (typeToken.lex == "int") 
                tf = DT_INT;
            else if (typeToken.lex == "double") 
                tf = DT_DOUBLE;
        }
    }
    std::string funcName = "";
    if (!nameNode->children[0]->token.lex.empty() && !nameNode->children.empty()) {
        funcName = nameNode->children[0]->token.lex;
        Token funcToken(T_ID, funcName);
        symbolTable.insert(funcToken);
    }
    post += dataStr(tf) + " " + funcName + " 2 DEFL\n";
}

void Semantic::rDescr(ParseTreeNode* node) {
    if (node->children.size() < 2 || !node) {
        error("Incorrect variable declaration");
        return;
    }
    DataType varType = DT_UNKNOWN;
    ParseTreeNode* typeNode = node->children[0];
    ParseTreeNode* varListNode = node->children[1];
    if (!typeNode->children.empty()) {
        Token typeToken = typeNode->children[0]->token;
        if (typeToken.lex == "int") 
            varType = DT_INT;
        else if (typeToken.lex == "double") 
            varType = DT_DOUBLE;
    }
    if (varType == DT_UNKNOWN) {
        error("Unknown variable type");
        return;
    }
    std::vector<std::string> vars;
    for (size_t i = 0; i < varListNode->children.size(); ++i) {
        ParseTreeNode* numNode = varListNode->children[i];
        if (!numNode->token.lex.empty()) {
            std::string varName = numNode->token.lex;
            if (ispr(varName)) 
                error("Variable redeclaration '" + varName + "' ");
            else {
                pr(varName, varType);
                vars.push_back(varName);
            }
        }
    }
    if (!vars.empty()) {
        post += dataStr(varType) + " ";
        for (size_t i = 0; i < vars.size(); ++i) 
            post += vars[i] + " ";
        post += std::to_string(1 + vars.size()) + " DECL\n";
    }
}

void Semantic::rDescriptions(ParseTreeNode* node) {
    for (size_t i = 0; i < node->children.size(); ++i) {
        rDescr(node->children[i]);
    }
}

void Semantic::rOp(ParseTreeNode* node) {
    if (node->children.size() < 2 || !node) {
        error("Incorrect assignment operator");
        return;
    }
    ParseTreeNode* numNode = node->children[0];
    ParseTreeNode* exprNode = node->children[1];
    std::string varName = numNode->token.lex;
    if (numNode->token.lex.empty()) {
        error("Incorrect variable name in assignment");
        return;
    }
    if (!ispr(varName)) {
        error("Variable '" + varName + "' not declared");
        return;
    }
    DataType lType = getType(varName);
    isppr(varName);
    post += varName + " ";
    DataType rType = rExpr(exprNode);
    if (rType != DT_ERROR && lType != DT_ERROR) {
        if (rType != lType)
            error("Type mismatch in assignment to '" + varName + "'");
    }
    post += "= \n";
}

void Semantic::rOperators(ParseTreeNode* node) {
    for (size_t i = 0; i < node->children.size(); ++i) {
        rOp(node->children[i]);
    }
}

DataType Semantic::rExpr(ParseTreeNode* node) {
    if (node->children.empty() || !node) {
        error("Incorrect expression");
        return DT_ERROR;
    }
    DataType lType = rTerm(node->children[0]);
    DataType rType = rExpr(node->children[2]);
    if (node->children.size() > 1) {
        std::string op;
        if (!node->children[1]->token.lex.empty()) 
            op = node->children[1]->token.lex;
        else 
            op = node->children[1]->name;
        if (lType != DT_ERROR && lType != rType && rType != DT_ERROR) {
            error("Type mismatch in expression with operator '" + op + "'");
        }
        post += op + " ";
        return lType;
    }
    return lType;
}

DataType Semantic::rTerm(ParseTreeNode* node) {
    if (node->children.empty() || !node) {
        error("Incorrect term");
        return DT_ERROR;
    }
    DataType lType = rSimpleExpr(node->children[0]);
    DataType rType = rTerm(node->children[2]);
    if (node->children.size() > 1) {
        std::string op;
        if (!node->children[1]->token.lex.empty()) op = node->children[1]->token.lex;
        else op = node->children[1]->name;
        if (lType != DT_ERROR && lType != rType && rType != DT_ERROR) {
            error("Type mismatch in term with operator '" + op + "'");
        }
        post += op + " ";
        return lType;
    }
    return lType;
}
DataType Semantic::rSimpleExpr(ParseTreeNode* node) {
    if (node->children.empty() || !node) {
        error("Incorrect simple expression");
        return DT_ERROR;
    }
    ParseTreeNode* fChild = node->children[0];
    if (fChild->name == "Expr") 
        return rExpr(fChild);
    if (fChild->name == "Id") {
        std::string varName = fChild->token.lex;
        if (varName.empty()) {
            error("Empty variable name");
            return DT_ERROR;
        }
        if (!ispr(varName)) {
            error("Variable '" + varName + "' not announced");
            return DT_ERROR;
        }
        isppr(varName);
        DataType varType = getType(varName);
        post += varName + " ";
        return varType;
    }
    if (fChild->name == "Const") {
        std::string constValue = fChild->token.lex;
        TokenType constType = fChild->token.type;
        post += constValue + " ";
        if (constType == T_CONST_INT) 
            return DT_INT;
        else if (constType == T_CONST_DOUBLE) 
            return DT_DOUBLE;
        error("Incorrect constant type");
        return DT_ERROR;
    }
    error("Incorrect simple expression structure");
    return DT_ERROR;
}

void Semantic::rEnd(ParseTreeNode* node) {
    if (node->children.empty() || !node) {
        error("Incorrect return statement");
        return;
    }
    ParseTreeNode* numNode = node->children[0];
    std::string varName = numNode->token.lex;
    if (varName.empty()) {
        error("Empty return value");
        return;
    }
    if (!ispr(varName)) {
        error("Returned variable '" + varName + "' not announced");
        return;
    }
    DataType returnType = getType(varName);
    if (returnType != tf) {
        error("Return type mismatch. Function returns " + dataStr(tf) + ", but returns " + dataStr(returnType));
        return;
    }
    isppr(varName);
    post += dataStr(returnType) + " " + varName + " 2 RET\n";
}