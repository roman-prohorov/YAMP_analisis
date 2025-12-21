#include "Lexical.h"
#include "Syntaxich.h"
#include "HashTable.h"
#include <iostream>
#include <fstream>

int main() {
    setlocale(LC_ALL, "RUS");
    std::ifstream testFile("input.txt");
    testFile.close();
    std::ofstream out("output.txt");
    //testLexer("input.txt", out);
    out << "=== SYNTACTIC ANALYSIS ===" << std::endl;
    Lexical lex("input.txt");
    Parsing parser(lex, out);
    bool fl = parser.parsing();
    out.close();
    std::cout << "=== ANALYSIS RESULTS ===" << std::endl;
    std::cout << "Всё прошло " << (fl ? "ок" : "не ок") << std::endl;
    std::cout << "Всё лежит в тхтишнике." << std::endl;
    if (fl) return 0;
    else return 1;
}