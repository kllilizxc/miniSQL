//
// Created by kllilizxc on 2016/6/9.
//

#include "Lexer.h"
#include <cctype>
#include <iostream>
#include <cstdlib>
#include "error.h"

using namespace std;

#define arrayLen(x) (sizeof(x) / sizeof(*x))

Lexer::Lexer(const char *instr) : index(0) {
    if (instr) {
        this->instr = new char[strlen(instr) + 1];
        strcpy(this->instr, instr);
    }
    advance();
}

void Lexer::advance() {
    int length = strlen(instr);
    nxtChar = (index + 1 >= length) ? (char) END : instr[index + 1];

    crtChar = (index >= length) ? (char) END : instr[index];

    index++;
}

void Lexer::skipWhiteSpace() {
    while (isspace(crtChar)) {
        crtChar = instr[index];
        if(index < strlen(instr)) index++;
        nxtChar = instr[index];
    }
}

const Token *Lexer::getNextToken() {
    try {
        //跳过whitespace
        skipWhiteSpace();
        if (crtChar == END) return new Token(EOI);

        //数字
        if (isdigit(crtChar)) {
            bool isFloat;
            double num = getNumber(&isFloat);
            if(isFloat) return dynamic_cast<const Token *>(new FloatToken(NUMBER, num));
            else return dynamic_cast<const Token *>(new IntToken(NUMBER, (int)num));
        }

        //keyword 或 变量 等
        if (isalpha(crtChar)) {
            char *keyword1 = getKeyword();
            for (int i = 0; i < arrayLen(skk); ++i) {
                if (!strcmp(keyword1, skk[i].key)) {
                    return new Token(skk[i].type);
                }
            }
            char *keyword2 = peekKeyword();
            for (int j = 0; j < arrayLen(dkk); ++j) {
                if (!strcmp(keyword1, dkk[j].firstKey) && !strcmp(keyword2, dkk[j].secondKey)) {
                    getKeyword();
                    return new Token(dkk[j].type);
                }
            }

            return dynamic_cast<const Token *>(new StringToken(ID, keyword1));
        }

        //标点符号
        char *symbols = peekCombinedSymbol();
        for (int k = 0; k < arrayLen(csmb); ++k) {
            if (!strcmp(symbols, csmb[k].symbol)) {
                getCombinedSymbol();
                return new Token(csmb[k].type);
            }
        }
        char symbol = getSymbol();
        for (int l = 0; l < arrayLen(smb); ++l) {
            if (symbol == smb[l].symbol) {
                Token *token = new Token(smb[l].type);
                return token;
            }
        }

        //属性
        if (symbol == '.') {
            char *keyword = getKeyword();
            if (!isalpha(keyword[0])) throw error("an alpha", keyword);
            return dynamic_cast<const Token *>(new StringToken(ATTR, keyword));
        }

        throw error("a symbol", symbols);
    } catch (error &e) {
        cout << endl << "Exit because of prevous errors." << endl;
        return new Token(ERROR);
    }
}

double Lexer::getNumber(bool *isFloat) {
    skipWhiteSpace();
    *isFloat = false;
    char *num = new char[20]; //TODO 可以写成可变
    int i = 0;
    do {
        num[i++] = crtChar;
        if(crtChar == '.') *isFloat = true;
        advance();
    } while (crtChar != END && (isdigit(crtChar) || crtChar == '.'));
    if (isalpha(crtChar)) {
        throw error("a digit", "an alpha");
    } else {
        num[i] = '\0';
        if(*isFloat) return atof(num);
        else return atoi(num);
    }

}

char *Lexer::getKeyword() {
    skipWhiteSpace();
    char *keyword = new char[20]; //TODO 可以写成可变
    int i = 0;
    do {
        keyword[i++] = crtChar;
        advance();
    } while (crtChar != END && isalnum(crtChar));
    keyword[i] = '\0';
    return keyword;
}

char *Lexer::peekKeyword() {
    char *keyword = new char[20]; //TODO 可以写成可变
    int i = 0;
    int off = -1;
    while (isspace(instr[index + off])) off++;
    do {
        keyword[i] = instr[index + off + i];
        i++;
    } while (instr[index + i] != END && isalnum(instr[index + i]));
    keyword[i] = '\0';
    return keyword;
}

char *Lexer::peekCombinedSymbol() {
    char *symbol = new char[5]; //TODO 可以写成可变
    int i = 0;
    int off = -1;
    while (isspace(instr[index + off])) off++;
    char nextChar = instr[index + off + i];
    do {
        symbol[i] = nextChar;
        i++;
        nextChar = instr[index + off +i];
    } while (nextChar != END && !isspace(nextChar) && !isalnum(nextChar));
    symbol[i] = '\0';
    return symbol;
}

char Lexer::getSymbol() {
    char symbol = crtChar;
    advance();
    return symbol;
}

char *Lexer::getCombinedSymbol() {
    char *symbol = new char[5]; //TODO 可以写成可变
    int i = 0;
    do {
        symbol[i++] = crtChar;
        advance();
    } while (crtChar != END && !isspace(crtChar) && !isalnum(crtChar));
    symbol[i] = '\0';
    return symbol;
}







