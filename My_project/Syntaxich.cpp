#include "Syntaxich.h"
#include <iostream>
#include <sstream>

ParseTreeNode::~ParseTreeNode() {
    for (size_t i = 0; i < children.size(); ++i) 
        delete children[i];
}

Parsing::Parsing(Lexical& l, std::ofstream& o): lex(l), out(o), erNow(false), cnter(0), pol(-1), prer(false) {
    advance();
}

Parsing::~Parsing() {
}

void Parsing::advance() {
    tekT = lex.getNextTok();
}
int Parsing::tz() {
    return pol - 1;
}
int Parsing::unk() {
    return pol;
}
bool Parsing::ser(int line) {
    if (line == pol || prer) return false;
    pol = line;
    return true;
}
void Parsing::proc(TokenType vsT, const std::string& vsL) {
    if (tekT.type == T_EOF) return;
    if (vsT == T_DELIMITER && vsL == ";") return;
    else if (vsT == T_DELIMITER && vsL == "}") skip(T_EOF);
    else {
        if (tekT.type != T_EOF) advance();
    }
}

void Parsing::skip(TokenType type, const std::string& lex) {
    while (tekT.type != T_EOF) {
        if ((tekT.lex == lex || lex.empty()) && tekT.type == type) {
            if (tekT.type != T_EOF) advance();
            return;
        }
        advance();
    }
}

void Parsing::match(TokenType vsT, const std::string& vsL) {
    if ((tekT.lex == vsL || vsL.empty()) && tekT.type == vsT) {
        advance();
        prer = false;
    }
    else {
        int erL;
        if (vsL == ";" && vsT == T_DELIMITER)
            erL = tz();
        else
            erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be ";
            if (!vsL.empty())
                ss << "'" << vsL << "'";
            else 
                ss << tokenTypeToString(vsT);
            ss << ", but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
        prer = true;
        proc(vsT, vsL);
    }
}

void Parsing::error(const std::string& input, int line) {
    erNow = true;
    cnter++;
    out << "Syntax Error: " << input << std::endl;
}

ParseTreeNode* Parsing::parseFunction() {
    ParseTreeNode* node = new ParseTreeNode("Function");
    node->children.push_back(parseBegin());
    node->children.push_back(parseDescriptions());
    node->children.push_back(parseOperators());
    node->children.push_back(parseEnd());
    return node;
}
ParseTreeNode* Parsing::parseBegin() {
    ParseTreeNode* node = new ParseTreeNode("Begin");
    node->children.push_back(parseType());
    node->children.push_back(parseFunctionName());
    match(T_DELIMITER, "(");
    match(T_DELIMITER, ")");
    match(T_DELIMITER, "{");
    return node;
}
ParseTreeNode* Parsing::parseEnd() {
    ParseTreeNode* node = new ParseTreeNode("End");
    if (tekT.lex != "return" || tekT.type != T_KEYWORD) {
        int erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be 'return', but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
        skip(T_EOF);
        return node; 
    }

    match(T_KEYWORD, "return");
    node->children.push_back(parseId());
    if (tekT.lex != ";" || tekT.type != T_DELIMITER) {
        int erL = tz();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be ';', but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
    }
    else match(T_DELIMITER, ";");
    if (tekT.lex != "}" || tekT.type != T_DELIMITER) {
        int erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Expected '}', but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
    }
    else match(T_DELIMITER, "}");
    return node;
}
ParseTreeNode* Parsing::parseFunctionName() {
    ParseTreeNode* node = new ParseTreeNode("FunctionName");
    node->children.push_back(parseId());
    return node;
}
ParseTreeNode* Parsing::parseDescriptions() {
    ParseTreeNode* node = new ParseTreeNode("Descriptions");
    node->children.push_back(parseDescr()); 
    while ((tekT.lex == "int" || tekT.lex == "double") && tekT.type == T_KEYWORD) {
        node->children.push_back(parseDescr());
    }
    return node;
}
ParseTreeNode* Parsing::parseOperators() {
    ParseTreeNode* node = new ParseTreeNode("Operators");
    node->children.push_back(parseOp()); 
    while (tekT.type == T_ID) {
        node->children.push_back(parseOp());
    }
    return node;
}
ParseTreeNode* Parsing::parseDescr() {
    ParseTreeNode* node = new ParseTreeNode("Descr");
    node->children.push_back(parseType());
    node->children.push_back(parseVarList());
    match(T_DELIMITER, ";");
    return node;
}
ParseTreeNode* Parsing::parseVarList() {
    ParseTreeNode* node = new ParseTreeNode("VarList");
    node->children.push_back(parseId()); 
    while (tekT.lex == "," && tekT.type == T_DELIMITER) {
        match(T_DELIMITER, ",");
        node->children.push_back(parseId());
    }
    return node;
}
ParseTreeNode* Parsing::parseType() {
    ParseTreeNode* node = new ParseTreeNode("Type");
    if (tekT.lex == "int" && tekT.type == T_KEYWORD) {
        ParseTreeNode* leaf = new ParseTreeNode("int", tekT);
        node->children.push_back(leaf);
        match(T_KEYWORD, "int");
    }
    else if (tekT.lex == "double" && tekT.type == T_KEYWORD) {
        ParseTreeNode* leaf = new ParseTreeNode("double", tekT);
        node->children.push_back(leaf);
        match(T_KEYWORD, "double");
    }
    else {
        int erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be 'int' or 'double', but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
    }
    return node;
}
ParseTreeNode* Parsing::parseOp() {
    ParseTreeNode* node = new ParseTreeNode("Op");
    node->children.push_back(parseId());
    match(T_OPERATOR, "=");
    node->children.push_back(parseExpr()); 
    if (tekT.lex != ";" || tekT.type != T_DELIMITER) {
        int erL = tz();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be ';', but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
    }
    else match(T_DELIMITER, ";");
    return node;
}
ParseTreeNode* Parsing::parseSimpleExpr() {
    ParseTreeNode* node = new ParseTreeNode("SimpleExpr");
    if (tekT.type == T_ID) 
        node->children.push_back(parseId());
    else if (tekT.type == T_DELIMITER) {
        match(T_DELIMITER, "(");
        node->children.push_back(parseExpr());
        match(T_DELIMITER, ")");
    }
    else if (tekT.type == T_CONST_INT || tekT.type == T_CONST_DOUBLE)
        node->children.push_back(parseConst());
    else { 
        int erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ":Should be identifier, constant, or '(', but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
        prer = true;
        proc(T_UNKNOWN);
    }
    return node;
}
ParseTreeNode* Parsing::parseExpr() {
    ParseTreeNode* node = new ParseTreeNode("Expr");
    node->children.push_back(parseTerm()); 
    if ((tekT.lex == "+" || tekT.lex == "-") && tekT.type == T_OPERATOR) {
        ParseTreeNode* opNode = new ParseTreeNode(tekT.lex, tekT);
        node->children.push_back(opNode);
        advance(); 
        node->children.push_back(parseExpr()); 
    }
    return node;
}

ParseTreeNode* Parsing::parseTerm() {
    ParseTreeNode* node = new ParseTreeNode("Term");
    node->children.push_back(parseSimpleExpr()); 
    if ((tekT.lex == "/" || tekT.lex == "*") && tekT.type == T_OPERATOR) {
        ParseTreeNode* opNode = new ParseTreeNode(tekT.lex, tekT); 
        node->children.push_back(opNode); 
        advance(); 
        node->children.push_back(parseTerm()); 
    }
    return node;
}

ParseTreeNode* Parsing::parseId() {
    if (tekT.type == T_ID) { 
        ParseTreeNode* node = new ParseTreeNode("Id", tekT);
        advance(); 
        return node;
    }
    else {  
        int erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be identifier, but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
        prer = true;
        proc(T_ID);
        return new ParseTreeNode("Id");
    }
}
ParseTreeNode* Parsing::parseConst() {
    if (tekT.type == T_CONST_DOUBLE || tekT.type == T_CONST_INT) {
        ParseTreeNode* node = new ParseTreeNode("Const", tekT);
        advance();
        return node;
    }
    else {
        int erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be constant, but found '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
        prer = true;
        proc(T_CONST_INT);
        return new ParseTreeNode("Const");
    }
}

bool Parsing::parsing() {
    pol = -1;
    erNow = false;
    cnter = 0;
    ParseTreeNode* root = parseFunction();
    if (tekT.type != T_EOF) {
        int erL = unk();
        if (ser(erL)) {
            std::stringstream ss;
            ss << "Line " << erL << ": Should be tokens at end of file: '" << tekT.lex << "'";
            error(ss.str(), erL);
        }
    }
    if (!erNow) {
        out << "Parse tree:" << std::endl;
        print(root, 0);
        out << std::endl;
    }
    out << "Syntax analysis " << (erNow ? "failed" : "completed successfully") << ". Errors: " << cnter << std::endl;
    delete root;
    return !erNow;
}

void Parsing::print(ParseTreeNode* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) 
        out << "  "; 
    out << node->name; 
    if (!node->token.lex.empty()) {
        out << " (" << node->token.lex << ")";
    }
    out << std::endl;
    for (size_t i = 0; i < node->children.size(); ++i) 
        print(node->children[i], depth + 1);
}

