//
// Created by kllilizxc on 2016/6/18.
//

#include "ConditionNode.h"


ConditionNode::ConditionNode(ConditionNode::conType type) : type(type), left(NULL), right(NULL) { }

ConditionNode::ConditionNode(int value, ConditionNode::conType type) : value(new int(value)), type(type), left(NULL), right(NULL){ }

ConditionNode::ConditionNode(float value, ConditionNode::conType type) : value(new float(value)), type(type) { }

ConditionNode::ConditionNode(string value, ConditionNode::conType type) : value(new string(value)), type(type), left(NULL), right(NULL) { }

int ConditionNode::getAttrValue() {
    return *((int *)value);
}

int ConditionNode::getIntValue() {
    return *((int *)value);
}

double ConditionNode::getFloatValue() {
    return *((double *)value);
}

string ConditionNode::getCharValue() {
    return *((string *)value);
}















