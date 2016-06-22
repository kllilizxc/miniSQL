#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#define BLOCK_SIZE_BYTE 4194304

using namespace std;

enum ATTRTYPE {
	INT, CHAR, FLOAT
};

int getSizeof(ATTRTYPE type) {//return bytes
	switch (type)
	{
	case INT:
	case FLOAT:
		return 4;
		break;
	default:
		return 0;
		break;
	}
}

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

uint32_t CharToInt(char* ch) {
	struct CTOI temp;
	temp.bytes[0] = ch[0];
	temp.bytes[1] = ch[1];
	temp.bytes[2] = ch[2];
	temp.bytes[3] = ch[3];
	return temp.ctoi;
}

float CharToFloat(char* ch) {
	struct CTOF temp;
	temp.bytes[0] = ch[0];
	temp.bytes[1] = ch[1];
	temp.bytes[2] = ch[2];
	temp.bytes[3] = ch[3];
	return temp.ctof;
}

struct TableCell {
	ATTRTYPE AttrType;
	union {
		uint32_t *ip;
		char *cp;
		float *fp;
	};
	struct TableCell *Next;
};

class TableRow {
public:
	char IsEmpty;
	struct TableCell *head;
	struct TableCell tail;
};



class Attr {
public:
	ATTRTYPE type;
	int charNum;
};

int main() {
	vector<Attr> tableMeta;

	uint32_t RecLength;
	uint32_t RecCount;
	string fileName = "t1_001.dbt";
	uint32_t TempRecOffset;
	int TempInnerOffset;
	TableRow *TempTableRow;
	struct TableCell **TempTableCell;
	vector<int> InnerOffsets;
	streampos size;
	char *memblock;
	ifstream TableFile(fileName.data(), ios::in | ios::binary | ios::ate);
	if (TableFile.is_open()) {
		size = TableFile.tellg();
		memblock = new char[size];
		TableFile.seekg(0, ios::beg);
		TableFile.read(memblock, size);
		TableFile.close();
		cout << "opened" << fileName << endl;
	}
	else {
		cout << "open failed" << fileName << endl;
	}
	//process
	cout << "fileName: " << fileName << '\n'
		<< "NextFileName: " << string(memblock, memblock + 32) << '\n'
		<< "PrevFileName: " << string(memblock + 32, memblock + 64) << '\n'
		<< "RecCount: " << (RecCount = CharToInt(memblock + 64)) << '\n'
		<< "HeadRecOffset: " << CharToInt(memblock + 68) << endl;


	//数据起始位置为72bytes
	//get record length and attr offset
	RecLength = 0;
	TempInnerOffset = 1;//one byte for empty mark
	for (vector<Attr>::iterator iter = tableMeta.begin(); iter != tableMeta.end(); ++iter) {
		InnerOffsets.push_back(TempInnerOffset);
		switch (iter->type) {
		case INT:
		case FLOAT:
			TempInnerOffset += getSizeof(iter->type);
			break;
		case CHAR:
			TempInnerOffset += iter->charNum;
			break;
		default:
			break;
		}
	}
	RecLength = TempInnerOffset;
	cout << "MaxRecCount: " << (BLOCK_SIZE_BYTE - 72) / RecLength << endl;
	cout << "table:" << endl << "------------" << endl;

	//process records
	TempRecOffset = 72;
	for (uint32_t i = 0; i < RecCount; ++i) {
		TempTableRow = new TableRow();
		TempTableCell = &(TempTableRow->head);
		if (memblock[TempRecOffset] == 1) {
			TempTableRow->IsEmpty = 1;
			TempTableRow->head = &(TempTableRow->tail);
		}
		else {
			TempTableRow->IsEmpty = 0;
			vector<Attr>::iterator it = tableMeta.begin();
			for (vector<int>::iterator iter = InnerOffsets.begin();
				iter != InnerOffsets.end();
				++iter, ++it) {

				*TempTableCell = new struct TableCell;
				(*TempTableCell)->AttrType = it->type;
				switch (it->type) {
				case INT:
					(*TempTableCell)->ip = new uint32_t(CharToInt(memblock + TempRecOffset + *iter));
					break;
				case FLOAT:
					(*TempTableCell)->fp = new float(CharToFloat(memblock + TempRecOffset + *iter));
					break;
				case CHAR:
					(*TempTableCell)->cp = new char[it->charNum];
					memcpy((*TempTableCell)->cp, memblock + TempRecOffset + *iter, it->charNum);
					break;
				}
				TempTableCell = &((*TempTableCell)->Next);
			}
		}
		(*TempTableCell)->Next = &(TempTableRow->tail);
		//Table.push_back(TempTableRow);
		//output TempTableRow
		for (struct TableCell *ptr = TempTableRow->head; ptr != &TempTableRow->tail; ptr = ptr->Next) {
			switch (ptr->AttrType) {
			case INT:
				cout << *(ptr->ip);
			case FLOAT:
				cout << *(ptr->fp);
			case CHAR:
				cout << ptr->cp;
			}
			cout << " | ";
		}
		cout << endl;
		TempRecOffset += RecLength;
	}
	//release
	delete[] memblock;
	return 0;
}