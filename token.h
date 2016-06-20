//
// Created by kllilizxc on 2016/6/9.
//

#ifndef MINISQL_TOKEN_H
#define MINISQL_TOKEN_H

#include <cstring>

typedef enum {
    EMPTY, ID, ATTR, //变量 0
    TABLE, NUMBER, INDEX, SELECT, DELETE, INSERT, FROM, WHERE, VALUES, ON, //命令关键词 3
    LP, RP, LBP, RBP, COMMA, SEMI, SQ, //标点符号 13
    EQ, NEQ, BT, LT, BET, LET, //比较符号 20
    AND, OR, //逻辑符号 26
    PRIMARY, UNIQUE, //表属性 28
    INT, FLOAT, CHAR, //数据类型 30
    EOI, ERROR //其他 33
} TokenType;

static struct singleKeyKeywords {
    TokenType type;
    const char *key;
} skk[] = {
        {SELECT, "select"},
        {UNIQUE, "unique"},
        {FROM,   "from"},
        {WHERE,  "where"},
        {INT,    "int"},
        {FLOAT,  "float"},
        {CHAR,   "char"},
        {VALUES, "values"},
        {ON,     "on"}
};

static struct doubleKeyKeywords {
    TokenType type;
    const char *firstKey;
    const char *secondKey;
} dkk[] = {
        {TABLE,   "create",     "table"},
        {INDEX,   "create",     "index"},
        {INSERT,  "insertInto", "into"},
        {DELETE,  "delete",     "from"},
        {PRIMARY, "primary",    "key"}
};

static struct symbols {
    TokenType type;
    const char symbol;
} smb[] = {
        {LP,    '('},
        {RP,    ')'},
        {LBP,   '{'},
        {RBP,   '}'},
        {COMMA, ','},
        {SEMI,  ';'},
        {EQ,    '='},
        {BT,    '>'},
        {LT,    '<'},
        {SQ,    '\''}
};

static struct combinedSymbols {
    TokenType type;
    const char *symbol;
} csmb[] = {
        {AND, "&&"},
        {OR,  "||"},
        {NEQ, "!="},
        {BET, ">="},
        {LET, "<="}
};

class Token {
public:
    TokenType type;

    Token(TokenType type = EMPTY);

    virtual const Token &operator=(const Token &another);

    virtual ~Token();
};

class IntToken : public Token {
public:
    int value;

    IntToken(TokenType type = EMPTY, int value = 0);

    const Token &operator=(const Token &another);
};

class FloatToken : public Token {
public:
    double value;

    FloatToken(TokenType type = EMPTY, double value = 0);

    const Token &operator=(const Token &another);
};

class StringToken : public Token {
public:
    char *value;

    StringToken(TokenType type = EMPTY, char *value = NULL);

    const Token &operator=(const Token &another);

    ~StringToken();
};


#endif //MINISQL_TOKEN_H
