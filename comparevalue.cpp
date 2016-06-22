#include "comparevalue.h"
#include <string>
bool CompareValue(string a, string b, ConditionNode::conType type) {
	switch (type)
	{
	case ConditionNode::EQ:
		return a == b;
	case ConditionNode::GT:
		return a > b;
	case ConditionNode::GEQ:
		return a >= b;
	case ConditionNode::LT:
		return a < b;
	case ConditionNode::LEQ:
		return a <= b;
	case ConditionNode::NEQ:
		return a != b;
	default:
		return false;
	}
}
bool CompareValue(int a, int b, ConditionNode::conType type) {
	switch (type)
	{
	case ConditionNode::EQ:
		return a == b;
	case ConditionNode::GT:
		return a > b;
	case ConditionNode::GEQ:
		return a >= b;
	case ConditionNode::LT:
		return a < b;
	case ConditionNode::LEQ:
		return a <= b;
	case ConditionNode::NEQ:
		return a != b;
	default:
		return false;
	}
}
bool CompareValue(float a, float b, ConditionNode::conType type) {
	switch (type)
	{
	case ConditionNode::EQ:
		return a == b;
	case ConditionNode::GT:
		return a > b;
	case ConditionNode::GEQ:
		return a >= b;
	case ConditionNode::LT:
		return a < b;
	case ConditionNode::LEQ:
		return a <= b;
	case ConditionNode::NEQ:
		return a != b;
	default:
		return false;
	}
}