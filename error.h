//
// Created by kllilizxc on 2016/6/10.
//

#ifndef MINISQL_ERROR_H
#define MINISQL_ERROR_H

#include "token.h"

class error {
public:
    error(const char *expt, const char *get);
    error(const char *expt, const TokenType get);
    error(const TokenType expt, const TokenType get);
    error(const char *msg);
};


#endif //MINISQL_ERROR_H
