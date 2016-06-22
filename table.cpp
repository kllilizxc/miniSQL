#include <cstring>
#include <iostream>
#include "table.h"
TableCell::TableCell() : type(TableMeta::INT), ip(new uint32_t(0)) { }

TableCell::TableCell(TableCell *tableCell) {
	type = tableCell->type;
	charNum = tableCell->charNum;
	switch (tableCell->type)
	{
		case TableMeta::INT:
			ip = new uint32_t(*(tableCell->ip));
			break;
		case TableMeta::FLOAT:
			fp = new float(*(tableCell->fp));
			break;
		case TableMeta::CHAR:
			cp = new char[tableCell->charNum];
			memcpy(cp, tableCell->cp, tableCell->charNum);
			break;
		default:
			break;
	}
	Next = NULL;
}

TableCell::TableCell(int value) : type(TableMeta::INT), ip(new uint32_t(value)) { }

TableCell::TableCell(const float value) : type(TableMeta::FLOAT), fp(new float(value)) { }

TableCell::TableCell(const char *value, int charNum) : type(TableMeta::CHAR), charNum(charNum), cp(new char[charNum + 1]) {
	strcpy(cp, value);
}

int TableCell::Int() {
	return (int)*ip;
}

float TableCell::Float() {
	return *fp;
}

char *TableCell::Char() {
	return cp;
}


TableRow::TableRow() : IsEmpty(0), end(NULL), tail(end), head(end) { }

void TableRow::append(TableCell *cell) {
	if(head == end) {
		head = cell;
		tail = cell;
	}
	else {
		tail->Next = cell;
		tail = tail->Next;
	}
	tail->Next = end;
}

TableRow* TableRow::DeepCopySelf() {
	TableRow *NewTableRow = new TableRow;
	TableCell *ptr = head;

	NewTableRow->IsEmpty = IsEmpty;
	if (!IsEmpty) {
		while (ptr != end) {
			NewTableRow->append(new TableCell(ptr));
			ptr = ptr->Next;
		}
	}
	return NewTableRow;
}

TableCell *TableRow::at(int index) {
	TableCell *ptr = head;
	if (IsEmpty) {
		return end;
	}
	for (int i = 0; i < index; ++i) {
		ptr = ptr->Next;
	}
	return ptr;
}

TableRow *TableRow::Filter(vector<int> attrIndex) {
	TableRow *TempTableRow = new TableRow();
	for(vector<int>::iterator iter = attrIndex.begin(); iter != attrIndex.end(); ++iter) {
		std::cout << *iter << std::endl;
		TempTableRow->append(new TableCell(at(*iter)));
	}
	return TempTableRow;
};