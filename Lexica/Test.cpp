#include "Lexical.h"
#include "HashTable.h"
#include "Semantic.h"
#include "Syntaxich.h"
#include <iostream>
#include <fstream>

int main() {
    setlocale(LC_ALL, "RUS");
    std::ofstream out("output.txt");
    out << "==== Lex Analysis ====\n";
    Lexical lex("input.txt");
    HashTable ht(211);
    bool fll = false;
    while (true) {
        Token t = lex.getNextTok();
        if (t.type == T_EOF) break;
        if (t.type == T_UNKNOWN) {
            fll = true;
            out << "Ошибка: хз, что за токен '" << t.lex << "\n";
            continue;
        }
        int index = ht.insert(t);
    }
    ht.print(out);
    if (fll) {
        out << "Лексический анализатор прервал свою работу из-за ошибки\n";
    }
    else {
        out << "Всё супер, ошибок нет\n";
    }
    out << "==== Synt Analysis ====\n";
    Lexical lexer("input.txt");
    Parsing parser(lex, out);
    ParseTreeNode* root = 0;
    bool fl = (root != nullptr);
    std::cout << "Всё прошло(1): " << (fl ? "ок" : "не ок") << std::endl;
    if (fl) {
        out << "==== Sem Analysis ====\n";
        Semantic semAnalyzer(out);
        bool fls = semAnalyzer.analyzis(root);
        std::cout << "Всё прошло(2): " << (fls ? "ок" : "не ок") << std::endl;
        delete root;
        if (!fls) {
            return 1;
        }
        std::cout << "Всё в тхтишнике" << std::endl;
        out.close();
    }