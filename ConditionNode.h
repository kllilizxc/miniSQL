//
// Created by kllilizxc on 2016/6/18.
//

#ifndef MINISQL_CONDITIONNODE_H
#define MINISQL_CONDITIONNODE_H

#include <string>

using namespace std;

class ConditionNode {
private:
    void *value;
public:
    enum conType {
        ATTR, INT, FLOAT, CHAR,
        EQ, GT, GEQ, LT, LEQ,
        AND, OR, NOT
    };
    ConditionNode *left;
    ConditionNode *right;
    ConditionNode::conType type;
    ConditionNode(ConditionNode::conType type);
    ConditionNode(int value, ConditionNode::conType type = INT);
    ConditionNode(double value, ConditionNode::conType type = FLOAT);
    ConditionNode(string value, ConditionNode::conType type = CHAR);
    int getAttrValue();
    int getIntValue();
    double getFloatValue();
    string getCharValue();
};


#endif //MINISQL_CONDITIONNODE_H
