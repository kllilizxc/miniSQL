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
        {Token::DOUBLE, "float"},
        {Token::CHAR,   "char"},
        {Token::VALUES, "values"},
        {Token::ON,     "on"}
};

doubleKeyKeywords Token::dkk[] = {
        {Token::TABLE,   "create",     "table"},
        {Token::INDEX,   "create",     "index"},
        {Token::INSERT,  "insert", "into"},
        {Token::DELETE,  "delete",     "from"},
        {Token::PRIMARY, "primary",    "key"}
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
        {Token::SQ,    '\''}
};

combinedSymbols Token::csmb[] = {
        {Token::AND, "&&"},
        {Token::OR,  "||"},
        {Token::NEQ, "!="},
        {Token::BET, ">="},
        {Token::LET, "<="}
};

Token::Token(Type type) : type(type) { }

Token::Token(int value) : type(INT), value(new int(value)) { }

Token::Token(double value) : type(DOUBLE), value(new double(value)) { }

Token::Token(string value, Type type) : type(type), value(new string(value)) { }

int Token::Int() const {
    return *((int *) value);
}

double Token::Double() const {
    return *((double *) value);
}

string Token::String() const {
    return *((string *) value);
}








