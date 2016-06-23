//
// Created by kllilizxc on 2016/6/9.
//

#include "token.h"

struct singleKeyKeywords Token::skk[] = {
        {Token::SELECT, "select"},
        {Token::UNIQUE, "unique"},
        {Token::FROM,   "from"},
        {Token::WHERE,  "where"},
        {Token::INT,    "int"},
        {Token::FLOAT,  "float"},
        {Token::CHAR,   "char"},
        {Token::VALUES, "values"},
        {Token::ON,     "on"}
};

doubleKeyKeywords Token::dkk[] = {
        {Token::TABLE,   "create",  "table"},
        {Token::INDEX,   "create",  "index"},
        {Token::INSERT,  "insert",  "into"},
        {Token::DELETE,  "delete",  "from"},
        {Token::PRIMARY, "primary", "key"}
};

symbols Token::smb[] = {
        {Token::LP,    '('},
        {Token::RP,    ')'},
        {Token::LBP,   '{'},
        {Token::RBP,   '}'},
        {Token::COMMA, ','},
        {Token::SEMI,  ';'},
        {Token::EQ,    '='},
        {Token::BT,    '>'},
        {Token::LT,    '<'},
        {Token::SQ,    '\''},
        {Token::NOT,   '!'},
        {Token::STAR,  '*'}
};

combinedSymbols Token::csmb[] = {
        {Token::AND, "and"},
        {Token::OR,  "or"},
        {Token::NEQ, "<>"},
        {Token::BET, ">="},
        {Token::LET, "<="}
};

Token::Token(Type type) : type(type) { }

Token::Token(int value) : type(INTEGER), value(new int(value)) { }

Token::Token(float value) : type(FLOATNUM), value(new float(value)) { }

Token::Token(string value, Type type) : type(type), value(new string(value)) { }

int Token::Int() const {
    return *((int *) value);
}

float Token::Float() const {
    return *((float *) value);
}

string Token::String() const {
    return *((string *) value);
}








