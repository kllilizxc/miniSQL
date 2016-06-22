#include <cstring>
#include <iostream>
#include "table.h"
TableCell::TableCell() : type(TableMeta::INT), ip(new uint32_t(0)) { }

TableCell::TableCell(TableCell *tableCell) {
	type = tableCell->type;
	charNum = tableCell->charNum;
	char *TempChar = NULL;
	switch (tableCell->type)
	{
		case TableMeta::INT:
			ip = new uint32_t(*(tableCell->ip));
			break;
		case TableMeta::FLOAT:
			fp = new float(*(tableCell->fp));
			break;
		case TableMeta::CHAR:
			TempChar = new char[tableCell->charNum];
			memcpy(TempChar, tableCell->cp, tableCell->charNum);
			cp = TempChar;
			break;
		default:
			break;
	}
	Next = NULL;
}

TableCell::TableCell(int value) : type(TableMeta::INT), ip(new uint32_t(value)) { }

TableCell::TableCell(const float value) : type(TableMeta::FLOAT), fp(new float(value)) { }

TableCell::TableCell(const char *value, int charNum) : type(TableMeta::CHAR), charNum(charNum), cp(new char) {
	cp = value;
}

TableCell::~TableCell() {
	switch (type) {
		case TableMeta::INT:
			delete ip;
			break;
		case TableMeta::FLOAT:
			delete fp;
			break;
		case TableMeta::CHAR:
			delete[] cp;
			break;
	}
}

int TableCell::Int() {
	return (int)*ip;
}

uint32_t TableCell::UInt32_t() {
	return (uint32_t)*ip;
}

float TableCell::Float() {
	return *fp;
}

const char *TableCell::Char() {
	return cp;
}

TableRow::~TableRow() {
	TableCell * TempTableCell;
	while (TempTableCell = head, TempTableCell != end) {
		head = head->Next;
		delete TempTableCell;
	}
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