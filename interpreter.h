#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include "Lexer.h"
#include "CatalogManager.h"
#include "ConditionNode.h"
#include <vector>

using namespace std;

class Interpreter {
private:
    const char *instr;
    Lexer *lexer;
    CatalogManager *catalogManager;
    inline const Token getNextToken(Token::Type type = Token::EMPTY);
    void createTable();
    void createIndex();
    void select();
    void insertInto();
    void deleteFrom();
    ConditionNode *buildConditionTree(TableMeta &table);
    ConditionNode *buildFactor(TableMeta &table);
    ConditionNode *buildTerm(TableMeta &table);
    bool compareType(ConditionNode::conType a, ConditionNode::conType b);
public:
    Interpreter(const char *instr = 0);
    void reset(const char *instr);
    ~Interpreter();
    void interpret();
};

#endif