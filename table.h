#pragma once
#include "tablemeta.h"
class TableCell {
public:
	union {
		uint32_t *ip;
		char *cp;
		float *fp;
	};
	TableMeta::ATTRTYPE type;
	int charNum;
	TableCell *Next;
	TableCell();
	TableCell(const int value);
	TableCell(const float value);
	TableCell(const char *value, int charNum);

	int Int();
	float Float();
	char *Char();
};

class TableRow {
public:
	char IsEmpty;
	TableCell *head;
	TableCell *tail;
	TableRow();
	void append(TableCell *cell);
	TableRow* DeepCopySelf();
	TableCell* at(int index);
};