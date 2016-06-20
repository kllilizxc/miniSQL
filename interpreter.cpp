#include <iostream>
#include <vector>
#include "interpreter.h"
#include "error.h"

using namespace std;

Interpreter::Interpreter(char *instr) : instr(instr) {
    catalogManager = new CatalogManager;
    lexer = new Lexer(this->instr);
}

Interpreter::~Interpreter() {
    delete lexer;
    delete catalogManager;
}

void Interpreter::interpret() {
    try {
        //create table { var type; }
        //create index .. on .. ()
        //select attr from id where ...
        //insertInto into id values (.., ..)
        //delete attr from id where ...
        const Token *token = lexer->getNextToken();
        switch (token->type) {
            case ERROR:
                break;
            case TABLE:
                creatTable();
                break;
            case INDEX:
                createIndex();
                break;
            case SELECT:
                select();
                break;
            case INSERT:
                insertInto();
                break;
            case DELETE:
                deleteFrom();
                break;
            case NUMBER: {
                const IntToken *intToken = dynamic_cast<const IntToken *>(token);
                if (intToken) cout << intToken->value;
                const FloatToken *floatToken = dynamic_cast<const FloatToken *>(token);
                if (floatToken) cout << floatToken->value;
                break;
            }
            case SQ: {
                const StringToken *stringToken = dynamic_cast<const StringToken *>(token);
                if (stringToken) cout << "'" << stringToken->value << "'";
                else throw error("an Id", token->type);
                expectNextToken(SQ);
                break;
            }
            default:
                break;
        }
    } catch (error &e) {
        cout << endl << "Exit because of prevous errors." << endl;
        return;
    }
}

bool Interpreter::expectNextToken(TokenType getType) {
    TokenType exptType = lexer->getNextToken()->type;
    if (getType == exptType) return true;
    else {
        throw error(exptType, getType);
    }
}

const StringToken *Interpreter::getNextStringToken() {
    return dynamic_cast<const StringToken *>(lexer->getNextToken());
}

const IntToken *Interpreter::getNextNumToken() {
    return dynamic_cast<const IntToken *>(lexer->getNextToken());
}

void Interpreter::creatTable() {
    //table name
    const StringToken *tableName = getNextStringToken();
    if (!tableName || (tableName->type != ID)) {
        throw error("tableName", "not a valid variable name");
    }
    int tableId = catalogManager->createTableMeta(tableName->value);
    // (
    expectNextToken(LBP);
    const StringToken *attrName = NULL;
    const Token *attrType = NULL;
    const Token *endToken = NULL;
    int count = -1;
    while (1) {
        count++;
        const Token *temp = getNextToken();
        if (temp->type == ID) {
            //attributes
            attrName = dynamic_cast<const StringToken *>(temp);
            if (count > 32) throw error("Too many attributes! Less than 32 expected!");
            attrType = getNextToken();
            switch (attrType->type) {
                case INT:
                case FLOAT:
                    catalogManager->addAttrToTableMeta(tableId, attrName->value, attrType->type);
                    break;
                case CHAR: {
                    expectNextToken(LP);
                    int charNum = getNextNumToken()->value;
                    expectNextToken(RP);
                    catalogManager->addAttrToTableMeta(tableId, attrName->value, attrType->type, charNum);
                    break;
                }
                default:
                    throw error("int or float or char", attrType->type);
            }
        } else if (temp->type == PRIMARY) {
            //primary key
            expectNextToken(LP);
            temp = getNextToken();
            while (temp->type != RP) {
                const StringToken *primaryKey = dynamic_cast<const StringToken *>(temp);
                catalogManager->setTableAttrProperty(tableId, primaryKey->value, PRIMARY);
                temp = getNextToken();
                if (temp->type == COMMA) {
                    temp = getNextToken();
                }
            }
        } else {
            throw error("attribute or primary", temp->type);
        }

        endToken = getNextToken();
        if (endToken->type == UNIQUE) {
            //unique
            catalogManager->setTableAttrProperty(tableId, attrName->value, UNIQUE);
            endToken = getNextToken();
        }
        if (endToken->type == COMMA) {
            continue;
        }
        else if (endToken->type == RBP) break;
        else {
            error(", or }", endToken->type);
            break;
        }

    }
    cout << endl;
}

void Interpreter::createIndex() {
    const StringToken *indexName = getNextStringToken();
    expectNextToken(ON);
    const StringToken *tableName = getNextStringToken();
    expectNextToken(LP);
    const Token *temp;
    vector<string> columns;
    while (1) {
        temp = getNextToken();
        if (temp->type == ID) {
            const StringToken *temp2 = dynamic_cast<const StringToken *>(temp);
            columns.push_back(temp2->value);
        }
        else if (temp->type == RP) break;
        else if (temp->type == COMMA) continue;
        else throw error("column or )", temp->type);
    }

    cout << "create index " << indexName->value << " on " << tableName->value << " on columns: ";
    for (int i = 0; i < columns.size(); ++i) cout << columns[i] << " ";
    cout << endl;
}

const Token *Interpreter::getNextToken() {
    return lexer->getNextToken();
}

bool Interpreter::elementInVector(vector<string> &v, string &e) {
    for (int i = 0; i < v.size(); ++i) {
        if (e == v[i]) return true;
    }
    return false;
}

void Interpreter::select() {
    const StringToken *attrName = NULL;
    const StringToken *tableName = NULL;

    vector<string> attrs;
    vector<string> tables;

    bool hasCondition = false;

    //attributes
    while (1) {
        attrName = getNextStringToken();
        attrs.push_back(attrName->value);
        const Token *temp = getNextToken();
        if (temp->type == COMMA) continue;
        else if (temp->type == FROM) break;
        else throw error(", or from", temp->type);
    }

    //tables
    while (1) {
        tableName = getNextStringToken();
        tables.push_back(tableName->value);
        const Token *temp = getNextToken();
        if (temp->type == COMMA) continue;
        else if (temp->type == WHERE) {
            hasCondition = true;
            break;
        }
        else if (temp->type == EOI) break;
        else throw error(", or where", temp->type);
    }

    //where
    ConditionNode *con;
    if (hasCondition) {
        con = buildConditionTree();
    }

    cout << "select ";
    for (int i = 0; i < attrs.size(); ++i) cout << attrs[i] << " ";
    cout << "from ";
    for (int j = 0; j < tables.size(); ++j) cout << tables[j] << " ";
    cout << endl;
}

void Interpreter::insertInto() {
    //table name
    const StringToken *tableName = getNextStringToken();
    expectNextToken(VALUES);
    expectNextToken(LP);
    const Token *temp;
    bool exit = false;
    cout << "insertInto into " << tableName->value << " with values: ";
    //values
    while (!exit) {
        temp = getNextToken();
        switch (temp->type) {
            case NUMBER: {
                const IntToken *temp2 = dynamic_cast<const IntToken *>(temp);
                if (temp2) cout << temp2->value;
                const FloatToken *temp3 = dynamic_cast<const FloatToken *>(temp);
                if (temp3) cout << temp3->value;
                break;
            }
            case SQ: {
                const StringToken *temp2 = getNextStringToken();
                cout << temp2->value;
                expectNextToken(SQ);
                break;
            }
            case COMMA:
                cout << ", ";
                continue;
            case RP:
                cout << endl;
                exit = true;
                break;
            default:
                throw error("value or , or )", temp->type);
        }
    }

}

void Interpreter::deleteFrom() {
    //table name
    const StringToken *tableName = getNextStringToken();

    cout << "delete from " << tableName->value << endl;

    //conditions
    const Token *temp = getNextToken();
    if (temp->type == WHERE) {
        //TODO
    }
    else if (temp->type == EOI) return;
    else throw error("where", temp->type);
}

ConditionNode *Interpreter::buildTerm() {
    const Token *temp = getNextToken();

    if(temp->type == LP) {
        return buildConditionTree();
        expectNextToken(RP);
    } else if(temp->type == ID) {
        const StringToken *id = dynamic_cast<const StringToken *>(temp);
        return new ConditionNode(id->value, ConditionNode::ATTR);
    } else if(temp->type == NUMBER) {
        const FloatToken *floatToken = dynamic_cast<const FloatToken *>(temp);
        if(floatToken) return new ConditionNode(floatToken->value);
        const IntToken *intValue = dynamic_cast<const IntToken *>(temp);
        if(intValue) return new ConditionNode(intValue->value);
        throw error("int or float", temp->type);
    } else if(temp->type == CHAR) {
        const StringToken *charValue = dynamic_cast<const StringToken *>(temp);
        return new ConditionNode(charValue->value);
    } else throw error("( or attr or number or char", temp->type);
}

ConditionNode *Interpreter::buildFactor() {
    ConditionNode *left = buildTerm();
    const Token *op = getNextToken();
    ConditionNode *node;
    switch (op->type) {
        case EQ: node = new ConditionNode(ConditionNode::EQ); break;
        case BT: node = new ConditionNode(ConditionNode::GT); break;
        case LT: node = new ConditionNode(ConditionNode::LT); break;
        case BET: node = new ConditionNode(ConditionNode::GEQ); break;
        case LET: node = new ConditionNode(ConditionNode::LEQ); break;
        default: throw error("= or > or < or >= or <=", op->type);
    }
    ConditionNode *right = buildTerm();
    node->left = left;
    node->right = right;
    return node;
}

ConditionNode *Interpreter::buildConditionTree() {
    ConditionNode *left = buildFactor();
    const Token *op = getNextToken();
    ConditionNode *node;
    switch (op->type) {
        case AND: node = new ConditionNode(ConditionNode::AND); break;
        case OR: node = new ConditionNode(ConditionNode::OR); break;
        default: throw error("&& or ||", op->type);
    }
    ConditionNode *right = buildFactor();
    node->left = left;
    node->right = right;
    return node;
}






























