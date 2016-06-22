#include <cstring>
#include "typeconvert.h"

uint32_t CharToInt(char* ch) {
	CTOI temp;
	temp.bytes[0] = ch[0];
	temp.bytes[1] = ch[1];
	temp.bytes[2] = ch[2];
	temp.bytes[3] = ch[3];
	return temp.ctoi;
}

float CharToFloat(char* ch) {
	CTOF temp;
	temp.bytes[0] = ch[0];
	temp.bytes[1] = ch[1];
	temp.bytes[2] = ch[2];
	temp.bytes[3] = ch[3];
	return temp.ctof;
}

char* IntToChar(uint32_t i) {
	CTOI *temp;
	temp = new CTOI;
	temp->ctoi = i;
	return temp->bytes;
}

char* FloatToChar(float f) {
	CTOF *temp;
	temp = new CTOF;
	temp->ctof = f;
	return temp->bytes;
}

char *StringToChar(string str, size_t size) {
	char *tempc = new char[size];
	memset(tempc, '\0', size);
	for (size_t i = 0; i < str.length() && i < size; ++i) {
		tempc[i] = str[i];
	}
	return tempc;
}

int getSizeof(TableMeta::ATTRTYPE type) {//return bytes
	switch (type)
	{
	case TableMeta::INT:
	case TableMeta::FLOAT:
		return 4;
		break;
	default:
		return 0;
		break;
	}
}