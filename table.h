#pragma once
#include "tablemeta.h"
class TableCell {
public:
	union {
		uint32_t *ip;
		const char *cp;
		float *fp;
	};
	TableMeta::ATTRTYPE type;
	int charNum;
	TableCell *Next;
	TableCell();
	TableCell(TableCell *tableCell);
	TableCell(const int value);
	TableCell(const float value);
	TableCell(const char *value, int charNum);
	~TableCell();

	int Int();
	uint32_t UInt32_t();
	float Float();
	const char *Char();
};

class TableRow {
public:
	char IsEmpty;
	TableCell *end;
	TableCell *tail;
	TableCell *head;
	TableRow();
	void append(TableCell *cell);
	TableRow* DeepCopySelf();
	TableCell* at(int index);
	TableRow *Filter(vector<int> attrIndex);
	~TableRow();
};