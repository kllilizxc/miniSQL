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
        ATTR_INT, ATTR_FLOAT, ATTR_CHAR,
        INT, FLOAT, CHAR,
        EQ, NEQ, GT, GEQ, LT, LEQ,
        AND, OR, NOT,
        LP, RP
    };
    ConditionNode *left;
    ConditionNode *right;
    ConditionNode::conType type;
    ConditionNode(ConditionNode::conType type);
    ConditionNode(int value, ConditionNode::conType type = ConditionNode::INT);
    ConditionNode(float value, ConditionNode::conType type = ConditionNode::FLOAT);
    ConditionNode(string value, ConditionNode::conType type = ConditionNode::CHAR);
    int getAttrValue();
    int getIntValue();
    double getFloatValue();
    string getCharValue();
};


#endif //MINISQL_CONDITIONNODE_H
