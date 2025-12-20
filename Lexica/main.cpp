#include "Lexical.h"
#include "HashTable.h"
#include <iostream>
#include <fstream>

int main() {
    setlocale(LC_ALL, "RUS");
    Lexical lex("input.txt");
    HashTable ht(211);
    std::ofstream out("output.txt");
    bool error = false;
    while (true) {
        Token t = lex.getNextTok();
        if (t.type == T_EOF) break;
        if (t.type == T_UNKNOWN) {
            error = true;
            out << "Ошибка: хз, что за токен'" << t.lex << "'\n";
            continue;
        }
        int index = ht.insert(t);
    }
    ht.print(out);
    if (error) out << "Лексический анализатор прервал свою работу из-за ошибка\n";
    else out << "Всё супер, ошибок нет\n";
    out.close();
    return 0;
}
