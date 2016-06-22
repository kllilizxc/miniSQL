#include <iostream>
#include <vector>
#include "interpreter.h"
#include "error.h"
#include "table.h"

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
        const Token token = lexer->getNextToken();
        switch (token.type) {
            case Token::ERROR:
                break;
            case Token::TABLE:
                createTable();
                break;
            case Token::INDEX:
                createIndex();
                break;
            case Token::SELECT:
                select();
                break;
            case Token::INSERT:
                insertInto();
                break;
            case Token::DELETE:
                deleteFrom();
                break;
            default:
                break;
        }
    } catch (error &e) {
        cout << endl << "Exit because of prevous errors." << endl;
        return;
    }
}

void Interpreter::createTable() {
    //table name
    const Token tableName = getNextToken();
    if (tableName.type != Token::ID) {
        throw error("tableName", "not a valid variable name");
    }
    int tableId = catalogManager->createTableMeta(tableName.String());
    // (
    getNextToken(Token::LBP);
    Token temp = getNextToken();
    Token attrName;
    int count = -1;
    while (1) {
        count++;
        if (temp.type == Token::ID) {
            //attributes
            attrName = temp;
            if (count > 32) throw error("Too many attributes! Less than 32 expected!");
            const Token attrType = getNextToken();
            switch (attrType.type) {
                case Token::INT:
                case Token::FLOAT:
                    catalogManager->addAttrToTableMeta(tableId, attrName.String(), attrType.type);
                    break;
                case Token::CHAR: {
                    getNextToken(Token::LP);
                    int charNum = getNextToken().Int();
                    getNextToken(Token::RP);
                    catalogManager->addAttrToTableMeta(tableId, attrName.String(), attrType.type, charNum);
                    break;
                }
                default:
                    throw error("int or float or char", attrType.type);
            }
        } else if (temp.type == Token::PRIMARY) {
            //primary key
            getNextToken(Token::LP);
            Token primaryKey = getNextToken(Token::ID);
            do {
                catalogManager->setTableAttrProperty(tableId, primaryKey.String(), Token::PRIMARY);
                temp = getNextToken();
                if (temp.type == Token::COMMA) {
                    primaryKey = getNextToken(Token::ID);
                }
            } while (temp.type != Token::RP);
        } else {
            throw error("attribute or primary", temp.type);
        }

        Token endToken = getNextToken();
        if (endToken.type == Token::UNIQUE) {
            //unique
            catalogManager->setTableAttrProperty(tableId, attrName.String(), Token::UNIQUE);
            endToken = getNextToken();
        }
        if (endToken.type == Token::COMMA) {
            temp = getNextToken();
            continue;
        }
        else if (endToken.type == Token::RBP) break;
        else {
            error(", or }", endToken.type);
            break;
        }

    }
    TableMeta *table = catalogManager->findTableMetaFromId(tableId);
    cout << endl;
}

void Interpreter::createIndex() {
    const Token indexName = getNextToken(Token::ID);
    getNextToken(Token::ON);
    const Token tableName = getNextToken(Token::ID);
    getNextToken(Token::LP);
    Token temp;
    vector<string> columns;
    while (1) {
        temp = getNextToken();
        if (temp.type == Token::ID) {
            columns.push_back(temp.String());
        }
        else if (temp.type == Token::RP) break;
        else if (temp.type == Token::COMMA) continue;
        else throw error("column or )", temp.type);
    }

    cout << "create index " << indexName.String() << " on " << tableName.String() << " on columns: ";
    for (int i = 0; i < columns.size(); ++i) cout << columns[i] << " ";
    cout << endl;
}

const Token Interpreter::getNextToken(Token::Type type) {
    Token result = lexer->getNextToken();
    if (type != Token::EMPTY && type != result.type) throw error(type, result.type);
    return result;
}

void Interpreter::select() {
    Token attrName;
    Token tableName;

    vector<string> attrs;
    vector<string> tables;

    bool hasCondition = false;

    //attributes
    while (1) {
        attrName = getNextToken(Token::ID);
        attrs.push_back(attrName.String());
        Token temp = getNextToken();
        if (temp.type == Token::COMMA) continue;
        else if (temp.type == Token::FROM) break;
        else throw error(", or from", temp.type);
    }

    //tables
    while (1) {
        tableName = getNextToken(Token::ID);
        tables.push_back(tableName.String());
        Token temp = getNextToken();
        if (temp.type == Token::COMMA) continue;
        else if (temp.type == Token::WHERE) {
            hasCondition = true;
            break;
        }
        else if (temp.type == Token::EOI) break;
        else throw error(", or where", temp.type);
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
    const Token tableName = getNextToken(Token::ID);
    getNextToken(Token::VALUES);
    getNextToken(Token::LP);
    TableMeta *tableMeta = catalogManager->findTableMetaFromId(catalogManager->getTableIdFromName(tableName.String()));
    TableRow tableRow;
    Token temp;
    bool exit = false;
    cout << "insertInto into " << tableName.String() << " with values: ";
    //values
    while (!exit) {
        temp = getNextToken();
        switch (temp.type) {
            case Token::INTEGER:
                cout << temp.Int();
                tableRow.append(new TableCell(temp.Int()));
                break;
            case Token::FLOATNUM:
                cout << temp.Float();
                tableRow.append(new TableCell(temp.Float()));
                break;
            case Token::SQ: {
                const Token temp2 = getNextToken();
                cout << "'" << temp2.String() << "'";
                getNextToken(Token::SQ);
                const char *data = temp2.String().data();
                tableRow.append(new TableCell(data, strlen(data) + 1));
                break;
            }
            case Token::COMMA:
                cout << ", ";
                continue;
            case Token::RP:
                cout << endl;
                exit = true;
                break;
            default:
                throw error("value or , or )", temp.type);
        }
    }
    //TODO tableMeta and tableRow ready

}

void Interpreter::deleteFrom() {
    //table name
    const Token tableName = getNextToken(Token::ID);
    TableMeta *tableMeta = catalogManager->findTableMetaFromId(catalogManager->getTableIdFromName(tableName.String()));

    cout << "delete from " << tableName.String() << endl;

    //conditions
    const Token temp = getNextToken();
    if (temp.type == Token::WHERE) {
        ConditionNode *con = buildConditionTree();
        //TODO tableMeta and con ready
    }
    else if (temp.type == Token::EOI) return;
    else throw error("where", temp.type);
}

ConditionNode *Interpreter::buildTerm() {
    Token temp = getNextToken();

    switch (temp.type) {
        case Token::LP: {
            return new ConditionNode(ConditionNode::LP);
        }
        case Token::ID: {
            return new ConditionNode(temp.String(), ConditionNode::ATTR);
        }
        case Token::INTEGER: {
            return new ConditionNode(temp.Int());
        }
        case Token::FLOATNUM: {
            return new ConditionNode(temp.Float());
        }
        case Token::SQ: {
            temp = getNextToken(Token::ID);
            ConditionNode *node = new ConditionNode(temp.String());
            getNextToken(Token::SQ);
            return node;
        }
        case Token::NOT: {
            return new ConditionNode(ConditionNode::NOT);
        }
        default:
            throw error("( or attr or number or char", temp.type);

    }
}

ConditionNode *Interpreter::buildFactor() {
    ConditionNode *left = buildTerm();
    ConditionNode *node;
    if (left->type == ConditionNode::NOT) {
        getNextToken(Token::LP);
        node = new ConditionNode(ConditionNode::NOT);
        node->left = buildConditionTree();
        return node;
    } else if (left->type == ConditionNode::LP) {
        node = buildConditionTree();
        return node;
    }
    const Token op = getNextToken();
    switch (op.type) {
        case Token::EQ:
            node = new ConditionNode(ConditionNode::EQ);
            break;
        case Token::BT:
            node = new ConditionNode(ConditionNode::GT);
            break;
        case Token::LT:
            node = new ConditionNode(ConditionNode::LT);
            break;
        case Token::BET:
            node = new ConditionNode(ConditionNode::GEQ);
            break;
        case Token::LET:
            node = new ConditionNode(ConditionNode::LEQ);
            break;
        default:
            throw error("= or > or < or >= or <=", op.type);
    }
    ConditionNode *right = buildTerm();
    node->left = left;
    node->right = right;
    return node;
}

ConditionNode *Interpreter::buildConditionTree() {
    ConditionNode *left = buildFactor();
    ConditionNode *node = NULL;
    Token op = getNextToken();
    while (1) {
        switch (op.type) {
            case Token::AND:
                node = new ConditionNode(ConditionNode::AND);
                break;
            case Token::OR:
                node = new ConditionNode(ConditionNode::OR);
                break;
            case Token::RP:
            case Token::EOI:
                return left;
            default:
                throw error("&& or ||", op.type);
        }
        ConditionNode *right = buildFactor();
        node->left = left;
        node->right = right;
        left = node;
        op = getNextToken();
    }
}





























