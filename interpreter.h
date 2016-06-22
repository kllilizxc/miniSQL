#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include "Lexer.h"
#include "CatalogManager.h"
#include "ConditionNode.h"
#include <vector>

using namespace std;

class Interpreter {
private:
    char *instr;
    Lexer *lexer;
    CatalogManager *catalogManager;
    inline const Token getNextToken(Token::Type type = Token::EMPTY);
    void createTable();
    void createIndex();
    void select();
    void insertInto();
    void deleteFrom();
    ConditionNode *buildConditionTree();
    ConditionNode *buildFactor();
    ConditionNode *buildTerm();
public:
    Interpreter(char *instr = 0);
    ~Interpreter();
    void interpret();
};

#endif