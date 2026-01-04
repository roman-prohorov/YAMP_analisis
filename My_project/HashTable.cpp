#include "HashTable.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Symin::Symin(const Token& t, int line)
    : token(t), varName(t.lex), tekstr(line), isp(false), datat(-1), next(nullptr) {
}

Symin::~Symin() {
}

HashTable::HashTable(int size) : capacity(size) {
    table = new Symin * [capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i] = nullptr;
    }
}

HashTable::~HashTable() {
    clear();
    delete[] table;
}

HashTable::HashTable(const HashTable& other) : capacity(other.capacity) {
    table = new Symin * [capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i] = nullptr;
    }
    for (int i = 0; i < capacity; ++i) {
        Symin* current = other.table[i];
        while (current) {
            Token copiedToken(current->token.type, current->token.lex);
            Symin* news = new Symin(copiedToken, current->tekstr);
            news->isp = current->isp;
            news->datat = current->datat;
            news->next = table[i];
            table[i] = news;
            current = current->next;
        }
    }
}

HashTable& HashTable::operator=(const HashTable& other) {
    if (this != &other) {
        clear();
        delete[] table;
        capacity = other.capacity;
        table = new Symin * [capacity];
        for (int i = 0; i < capacity; ++i) {
            table[i] = nullptr;
        }
        for (int i = 0; i < capacity; ++i) {
            Symin* cur = other.table[i];
            while (cur) {
                Token copy(cur->token.type, cur->token.lex);
                Symin* news = new Symin(copy, cur->tekstr);
                news->isp = cur->isp;
                news->datat = cur->datat;
                news->next = table[i];
                table[i] = news;
                cur = cur->next;
            }
        }
    }
    return *this;
}

unsigned long HashTable::hash(const std::string& str) {
    unsigned long h = 5381;
    for (size_t i = 0; i < str.size(); ++i) {
        h = ((h << 5) + h) + str[i];
    }
    return h % capacity;
}

int HashTable::insert(const Token& token, int line) {
    std::string lex = token.lex;
    if (lex.empty()) {
        return -1;
    }
    unsigned long h = hash(lex);
    Symin* cur = table[h];
    while (cur) {
        if (cur->token.lex == lex) {
            cur->token.type = token.type;
            cur->tekstr = line;
            return h;
        }
        cur = cur->next;
    }
    Symin* news = new Symin(token, line);
    news->next = table[h];
    table[h] = news;
    return h;
}

bool HashTable::contains(const std::string& lex) {
    if (lex.empty()) {
        return false;
    }
    unsigned long h = hash(lex);
    Symin* cur = table[h];
    while (cur) {
        if (cur->token.lex == lex) {
            return true;
        }
        cur = cur->next;
    }
    return false;
}

Token* HashTable::find(const std::string& lex) {
    if (lex.empty()) {
        return nullptr;
    }
    unsigned long h = hash(lex);
    Symin* cur = table[h];
    while (cur) {
        if (cur->token.lex == lex) {
            return &(cur->token);
        }
        cur = cur->next;
    }
    return nullptr;
}

Symin* HashTable::findS(const std::string& lex) {
    if (lex.empty()) {
        return nullptr;
    }
    unsigned long h = hash(lex);
    Symin* cur = table[h];
    while (cur) {
        if (cur->token.lex == lex) {
            return cur;
        }
        cur = cur->next;
    }
    return nullptr;
}

bool HashTable::remove(const std::string& lex) {
    if (lex.empty()) {
        return false;
    }
    unsigned long h = hash(lex);
    Symin* cur = table[h];
    Symin* prev = nullptr;
    while (cur) {
        if (cur->token.lex == lex) {
            if (prev) {
                prev->next = cur->next;
            }
            else {
                table[h] = cur->next;
            }
            delete cur;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

void HashTable::clear() {
    for (int i = 0; i < capacity; ++i) {
        Symin* cur = table[i];
        while (cur) {
            Symin* temp = cur;
            cur = cur->next;
            delete temp;
        }
        table[i] = nullptr;
    }
}

bool HashTable::setDatat(const std::string& lex, int type) {
    Symin* sym = findS(lex);
    if (sym) {
        sym->datat = type;
        return true;
    }
    return false;
}

int HashTable::getDatat(const std::string& lex) {
    Symin* sym = findS(lex);
    if (sym) {
        return sym->datat;
    }
    return -1; 
}

bool HashTable::marka(const std::string& lex) {
    Symin* sym = findS(lex);
    if (sym) {
        sym->isp = true;
        return true;
    }
    return false;
}

bool HashTable::isp(const std::string& lex) {
    Symin* sym = findS(lex);
    if (sym) {
        return sym->isp;
    }
    return false;
}

int HashTable::getTekstr(const std::string& lex) {
    Symin* sym = findS(lex);
    if (sym) {
        return sym->tekstr;
    }
    return -1;
}

std::vector<Symin*> HashTable::allin() {
    std::vector<Symin*> symbols;
    for (int i = 0; i < capacity; ++i) {
        Symin* cur = table[i];
        while (cur) {
            symbols.push_back(cur);
            cur = cur->next;
        }
    }
    return symbols;
}

std::vector<std::string> HashTable::allinn() {
    std::vector<std::string> names;
    for (int i = 0; i < capacity; ++i) {
        Symin* cur = table[i];
        while (cur) {
            if (!cur->token.lex.empty()) {
                names.push_back(cur->token.lex);
            }
            cur = cur->next;
        }
    }
    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());
    return names;
}

void HashTable::print(std::ostream& os) {
    os << "Hash Table (Capacity: " << capacity << ")" << std::endl;
    os << "Index | Type | Lexeme | Hash" << std::endl;
    os << "--------------------------------" << std::endl;
    int totalElements = 0;
    for (int i = 0; i < capacity; ++i) {
        Symin* cur = table[i];
        while (cur) {
            os << i << " | " << tokenTypeToStr(cur->token.type) << " | "
                << cur->token.lex << " | " << hash(cur->token.lex) << std::endl;
            cur = cur->next;
            totalElements++;
        }
    }
    os << "Total elements: " << totalElements << std::endl;
}

void HashTable::printSem(std::ostream& os) {
    os << "Symbol Table (Semantic Information)" << std::endl;
    os << "Name | Type | DataType | Line | Used" << std::endl;
    os << "----------------------------------------" << std::endl;
    std::vector<Symin*> sym = allin();
    for (Symin* sym : sym) {
        std::string dataTypeStr;
        switch (sym->datat) {
        case 0: 
            dataTypeStr = "int"; 
            break;
        case 1:
            dataTypeStr = "double"; 
            break;
        case -1: 
            dataTypeStr = "undefined";
            break;
        default:
            dataTypeStr = "unknown(" + std::to_string(sym->datat) + ")";
        }
        os << sym->token.lex << " | " << tokenTypeToStr(sym->token.type) << " | "
            << dataTypeStr << " | " << sym->tekstr << " | " << (sym->isp ? "yes" : "no") << std::endl;
    }
}

int HashTable::size() const {
    int size = 0;
    for (int i = 0; i < capacity; ++i) {
        Symin* cur = table[i];
        while (cur) {
            size++;
            cur = cur->next;
        }
    }
    return size;
}

bool HashTable::empty() const {
    for (int i = 0; i < capacity; ++i) {
        if (table[i] != nullptr) {
            return false;
        }
    }
    return true;
}

std::string HashTable::tokenTypeToStr(TokenType t) {
    switch (t) {
    case T_INT: 
        return "int";
    case T_DOUBLE: 
        return "double";
    case T_ID: 
        return "id";
    case T_KEYWORD: 
        return "keyword";
    case T_OPERATOR:
        return "operator";
    case T_CONST_INT: 
        return "int_const";
    case T_CONST_DOUBLE:
        return "double_const";
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
