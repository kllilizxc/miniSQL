//
// Created by kllilizxc on 2016/6/9.
//

#include "Lexer.h"
#include <iostream>
#include <cstdlib>
#include "error.h"

using namespace std;


Lexer::Lexer(const char *instr){
    reset(instr);
}

void Lexer::advance() {
    if(!instr)
        return;
    int length = strlen(instr);

    crtChar = (index >= length) ? (char) END : instr[index];

    index++;
}

void Lexer::skipWhiteSpace() {
    while (isspace(crtChar)) {
        crtChar = instr[index];
        if(index < strlen(instr)) index++;
    }
}

const Token Lexer::getNextToken() {
    try {
        //跳过whitespace
        skipWhiteSpace();
        if (crtChar == END) return Token(Token::EOI);

        //数字
        if (isdigit(crtChar)) {
            bool isFloat;
            float num = getNumber(&isFloat);
            if(isFloat) return Token(num);
            else return Token(static_cast<int>(num));
        }

        //keyword 或 变量 等
        if (isalpha(crtChar)) {
            char *keyword1 = getKeyword();
            for (int i = 0; i < Token::SINGLE_KEY_KEYWORDS_NUM; ++i) {
                if (!strcmp(keyword1, Token::skk[i].key)) {
                    return Token(Token::skk[i].type);
                }
            }
            char *keyword2 = peekKeyword();
            for (int j = 0; j < Token::DOUBLE_KEY_KEYWORDS_NUM; ++j) {
                if (!strcmp(keyword1, Token::dkk[j].firstKey) && !strcmp(keyword2, Token::dkk[j].secondKey)) {
                    getKeyword();
                    return Token(Token::dkk[j].type);
                }
            }

            return Token(keyword1, Token::ID);
        }

        if(crtChar == '\'') {
            //string
            char str[100];
            int i = 0;
            advance();
            while (crtChar != '\'') {
                str[i++] = crtChar;
                advance();
            }
            str[i] = '\0';
            advance();
            return Token(str, Token::STRING);
        }
        //标点符号
        char *symbols = peekCombinedSymbol();
        for (int k = 0; k < Token::COMBINED_SYMBOLS_NUM; ++k) {
            if (!strcmp(symbols, Token::csmb[k].symbol)) {
                getCombinedSymbol();
                return Token(Token::csmb[k].type);
            }
        }

        char symbol = getSymbol();
        for (int l = 0; l < Token::SYMBOLS_NUM; ++l) {
            if (symbol == Token::smb[l].symbol) {
                return Token(Token::smb[l].type);
            }
        }

        //属性
        if (symbol == '.') {
            char *keyword = getKeyword();
            if (!isalpha(keyword[0])) throw error("an alpha", keyword);
            return Token(keyword, Token::ATTR);
        }

        throw error("a symbol", symbols);
    } catch (error &e) {
        cout << endl << "Exit because of prevous errors." << endl;
        return Token(Token::ERROR);
    }
}

float Lexer::getNumber(bool *isFloat) {
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
        if(*isFloat) return static_cast<float>(atof(num));
        else return static_cast<float >(atoi(num));
    }

}

char *Lexer::getKeyword() {
    skipWhiteSpace();
    char *keyword = new char[20]; //TODO 可以写成可变
    int i = 0;
    do {
        keyword[i++] = crtChar;
        advance();
    } while (crtChar != END && (isalnum(crtChar) || crtChar == '-'));
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
    } while (nextChar != END && !isspace(nextChar) && !isalnum(nextChar) && (nextChar != '\''));
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
    } while (crtChar != END && !isspace(crtChar) && !isalnum(crtChar) && (crtChar != '\''));
    symbol[i] = '\0';
    return symbol;
}

void Lexer::reset(const char *instr) {
    if(!instr)
        return;
    index = 0;
    if (instr) {
        this->instr = new char[strlen(instr) + 1];
        strcpy(this->instr, instr);
    }
    advance();
}









