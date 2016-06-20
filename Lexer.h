//
// Created by kllilizxc on 2016/6/9.
//

#ifndef MINISQL_LEXER_H
#define MINISQL_LEXER_H

#include <cstring>
#include "token.h"

#define END -1

class Lexer {
private:
    char *instr;
    int index;
    char crtChar;
    char nxtChar;
    void skipWhiteSpace();
    double getNumber(bool *isFloat);
    char *getKeyword();
    char *peekKeyword();
    char *peekCombinedSymbol();
    char *getCombinedSymbol();
    char getSymbol();
    void advance();
public:
    Lexer(const char *instr = 0);
    const Token *getNextToken();

};

#endif //MINISQL_LEXER_H
