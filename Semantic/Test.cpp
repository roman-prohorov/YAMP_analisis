#include "Lexical.h"
#include "HashTable.h"
#include "Semantic.h"
#include "Syntaxich.h"
#include <iostream>
#include <fstream>

int main() {
    std::ofstream out("output.txt");
    out << "===  SYNTACTIC ANALYSIS ===" << std::endl;
    Lexical lex("input.txt");
    Parsing parser(lex, out);
    ParseTreeNode* root = 0; // parser.parsing();
    bool fl = (root != nullptr);
    std::cout << "Всё прошло(1): " << (fl ? "ок" : "не ок") << std::endl;
    if (fl) {
        out << std::endl << "=== SEMANTIC ANALYSIS ===" << std::endl;
        Semantic semAnalyzer(out);
        bool fls = semAnalyzer.analyzis(root);
        std::cout << "Всё прошло(2): " << (fls ? "ок" : "не ок") << std::endl;
        delete root;
        if (!fls) return 1;
    }
    else return 1;
    std::cout << "Всё в тхтишнике" << std::endl;
    out.close();
    return 0;
}