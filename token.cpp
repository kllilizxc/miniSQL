//
// Created by kllilizxc on 2016/6/9.
//

#include "token.h"

Token::Token(TokenType type) : type(type) { }

Token::~Token() { }

const Token &Token::operator=(const Token &another) {
    this->type = another.type;
    return *this;
}

IntToken::IntToken(TokenType type, int value) : Token(type), value(value) { }

const Token &IntToken::operator=(const Token &another) {
    this->type = another.type;
    const IntToken *temp = dynamic_cast<const IntToken *>(&another);
    if (temp) this->value = temp->value;
    return *this;
}

StringToken::StringToken(TokenType type, char *value) : Token(type), value(value) {
}

const Token &StringToken::operator=(const Token &another) {
    this->type = another.type;
    this->value = NULL;
    const StringToken *temp = dynamic_cast<const StringToken *>(&another);
    if (temp && temp->value) {
        this->value = new char[strlen(temp->value) + 1];
        strcpy(this->value, temp->value);
    }
    return *this;
}

StringToken::~StringToken() {
    delete[] value;
}

FloatToken::FloatToken(TokenType type, double value) : Token(type), value(value) { }

const Token &FloatToken::operator=(const Token &another) {
    this->type = another.type;
    const FloatToken *temp = dynamic_cast<const FloatToken *>(&another);
    if (temp) this->value = temp->value;
    return *this;
}



