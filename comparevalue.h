#pragma once
#include <string>
#include "conditionnode.h"
bool CompareValue(int a, int b, ConditionNode::conType type);
bool CompareValue(float a, float b, ConditionNode::conType type);
bool CompareValue(string a, string b, ConditionNode::conType type);