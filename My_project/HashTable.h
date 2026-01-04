#pragma once

#include "Token.h"
#include <string>
#include <ostream>
#include <vector>

struct Symin {
    Token token;                
    std::string varName;                 
    bool isp;                
    int datat;
    int tekstr;
    Symin* next;          
    Symin(const Token& t, int line = 0);
    ~Symin();
};

class HashTable {
private:
    Symin** table;         
    int capacity;              
    unsigned long hash(const std::string& str);
public:
    HashTable(int size = 211);
    ~HashTable();
    HashTable(const HashTable& other);            
    HashTable& operator=(const HashTable& other);
    int insert(const Token& token, int line = 0);
    bool contains(const std::string& lex);
    Token* find(const std::string& lex);
    Symin* findS(const std::string& lex);
    bool remove(const std::string& lex);
    void clear();
    bool setDatat(const std::string& lex, int type);
    int getDatat(const std::string& lex);
    bool marka(const std::string& lex);
    bool isp(const std::string& lex);
    int getTekstr(const std::string& lex);
    std::string tokenTypeToStr(TokenType t);
    std::vector<Symin*> allin();
    std::vector<std::string> allinn();
    void print(std::ostream& os);
    void printSem(std::ostream& os);
    int Capacity() const { 
        return capacity; 
    }
    int size() const;
    bool empty() const;
};

