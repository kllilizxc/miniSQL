#pragma once
#include "tablemeta.h"
struct CTOI {
	union {
		char bytes[4];
		uint32_t ctoi;
	};
};

struct CTOF {
	union {
		char bytes[4];
		float ctof;
	};
};

uint32_t CharToInt(char* ch);

float CharToFloat(char* ch);

char* IntToChar(uint32_t i);

char* FloatToChar(float f);

char *StringToChar(string str, size_t size);

int getSizeof(TableMeta::ATTRTYPE type);
