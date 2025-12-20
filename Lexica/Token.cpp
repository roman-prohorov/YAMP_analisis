#include "Token.h"

std::string tokenTypeToStr(TokenType t) {
    switch (t) {
    case T_INT:
        return "int";
    case T_DOUBLE:
        return "double";
    case T_CONST_INT:
        return "int_const";
    case T_CONST_DOUBLE:
        return "double_const";
    case T_ID:
        return "id";
    case T_KEYWORD:
        return "keyword";
    case T_OPERATOR:
        return "operator";
    case T_DELIMITER:
        return "delimiter";
    case T_UNKNOWN:
        return "unknown";
    case T_EOF:
        return "End_of_file";
    default:
        return "unknown";
    }
}