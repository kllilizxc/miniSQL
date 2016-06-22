#include <cstring>
#include "table.h"
TableCell::TableCell() : type(TableMeta::INT), ip(new uint32_t(0)) { }

TableCell::TableCell(int value) : type(TableMeta::INT), ip(new uint32_t(value)) { }

TableCell::TableCell(const float value) : type(TableMeta::FLOAT), fp(new float(value)) { }

TableCell::TableCell(const char *value, int charNum) : type(TableMeta::CHAR), charNum(charNum), cp(new char[charNum + 1]) {
	strcpy(cp, value);
}

int TableCell::Int() {
	return *ip;
}

float TableCell::Float() {
	return *fp;
}

char *TableCell::Char() {
	return cp;
}


TableRow::TableRow() : IsEmpty(0), head(new TableCell(0)), tail(head) {
	tail->Next = NULL;
}

void TableRow::append(TableCell *cell) {
	tail->Next = cell;
	tail = cell;
	tail->Next = NULL;
}

TableRow* TableRow::DeepCopySelf() {
	TableRow *NewTableRow = new TableRow;

	NewTableRow->IsEmpty = IsEmpty;
	if (IsEmpty) {
		NewTableRow->head = NewTableRow->tail;
	}
	else {
		TableCell *ptr = head;
		TableCell **pptr = &(NewTableRow->head);
		while (ptr != tail) {
			*pptr = new TableCell();
			(*pptr)->type = ptr->type;
			(*pptr)->charNum = ptr->charNum;
			switch (ptr->type)
			{
			case TableMeta::INT:
				(*pptr)->ip = new uint32_t(*(ptr->ip));
				break;
			case TableMeta::FLOAT:
				(*pptr)->fp = new float(*(ptr->fp));
				break;
			case TableMeta::CHAR:
				(*pptr)->cp = new char[ptr->charNum];
				memcpy((*pptr)->cp, ptr->cp, ptr->charNum);
				break;
			default:
				break;
			}
			ptr = ptr->Next;
			pptr = &((*pptr)->Next);
		}
		(*pptr)->Next = NewTableRow->tail;
	}
	return NewTableRow;
}

TableCell *TableRow::at(int index) {
	TableCell *ptr = head;
	if (IsEmpty) {
		return tail;
	}
	for (int i = 0; i < index; ++i) {
		ptr = ptr->Next;
	}
	return ptr;
}