//
// Created by kllilizxc on 2016/6/9.
//

#ifndef MINISQL_TOKEN_H
#define MINISQL_TOKEN_H

#include <cstring>
#include <string>

using namespace std;

struct singleKeyKeywords;
struct doubleKeyKeywords;
struct symbols;
struct combinedSymbols;

class Token {
private:
    void *value;
public:
    static const int SINGLE_KEY_KEYWORDS_NUM = 9;
    static const int DOUBLE_KEY_KEYWORDS_NUM = 5;
    static const int SYMBOLS_NUM = 11;
    static const int COMBINED_SYMBOLS_NUM = 5;

    enum Type {
        EMPTY, ID, ATTR, INTEGER, FLOATNUM, //变量 0
        TABLE, INDEX, SELECT, DELETE, INSERT, FROM, WHERE, VALUES, ON, //命令关键词 5
        LP, RP, LBP, RBP, COMMA, SEMI, SQ, //标点符号 14
        EQ, NEQ, BT, LT, BET, LET, //比较符号 21
        AND, OR, NOT, //逻辑符号 27
        PRIMARY,UNIQUE, //表属性 30
        INT, FLOAT, CHAR, //数据类型 32
        EOI, ERROR //其他 35
    };

    static singleKeyKeywords skk[];

    static doubleKeyKeywords dkk[];

    static symbols smb[];

    static combinedSymbols csmb[];

    Type type;

    Token(Type type = EMPTY);

    Token(int value);

    Token(float value);

    Token(string value, Type type = Token::CHAR);

    int Int() const;

    float Float() const;

    string String() const;
};

struct singleKeyKeywords {
    Token::Type type;
    const char *key;
};

struct doubleKeyKeywords {
    Token::Type type;
    const char *firstKey;
    const char *secondKey;
};

struct symbols {
    Token::Type type;
    const char symbol;
};

struct combinedSymbols {
    Token::Type type;
    const char *symbol;
};

#endif //MINISQL_TOKEN_H
