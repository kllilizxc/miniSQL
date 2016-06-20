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
    bool expectNextToken(TokenType getType);
    inline const Token* getNextToken();
    inline const StringToken * getNextStringToken();
    inline const IntToken * getNextNumToken();
    static bool elementInVector(vector<string> &v, string &e);
    void creatTable();
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