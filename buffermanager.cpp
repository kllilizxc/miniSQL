#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include "tablemeta.h"
#include "table.h"
#include "typeconvert.h"
#include "buffermanager.h"
#define BLOCK_SIZE_BYTE 4194304
#define BLOCK_HEAD_OFFSET 72

using namespace std;

BufferTable::BufferMap BufferTable::BuffList;

BufferTable::BufferTable(string fileName, TableMeta tableMeta) {
	cout << "-------------" << endl;
	IsDirty = false;
	RefNum = 0;
	attrs = tableMeta.attrs;
	//read file
	uint32_t TempRecOffset;
	uint32_t TempInnerOffset;
	TableRow *TempTableRow;
	TableCell **TempTableCell;
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
	FileName = fileName;
	NextFileName = string(memblock, memblock + 32);
	PrevFileName = string(memblock + 32, memblock + 64);
	RecCount = CharToInt(memblock + 64);
	HeadRecOffset = CharToInt(memblock + 68);
	//数据起始位置为72bytes
	//get record length and attr offset
	RecLength = 0;
	TempInnerOffset = 1;//one byte for empty mark
	for (vector<Attr>::iterator iter = attrs.begin(); iter != attrs.end(); ++iter) {
		InnerOffsets.push_back(TempInnerOffset);
		switch (iter->type) {
		case TableMeta::INT:
		case TableMeta::FLOAT:
			TempInnerOffset += getSizeof(iter->type);
			break;
		case TableMeta::CHAR:
			TempInnerOffset += iter->charNum;
			break;
		default:
			break;
		}
	}
	RecLength = TempInnerOffset;
	MaxRecCount = (BLOCK_SIZE_BYTE - BLOCK_HEAD_OFFSET) / RecLength;

	//process records
	TempRecOffset = BLOCK_HEAD_OFFSET;
	for (uint32_t i = 0; i < RecCount; ++i) {
		TempTableRow = new TableRow();
		TempTableCell = &(TempTableRow->head);
		if (memblock[TempRecOffset] == 1) {
			TempTableRow->IsEmpty = 1;
		}
		else {
			TempTableRow->IsEmpty = 0;
			vector<Attr>::iterator it = attrs.begin();
			for (vector<uint32_t>::iterator iter = InnerOffsets.begin();
				iter != InnerOffsets.end();
				++iter, ++it) {

				*TempTableCell = new TableCell;
				(*TempTableCell)->type = it->type;
				switch (it->type) {
				case TableMeta::INT:
					(*TempTableCell)->ip = new uint32_t(CharToInt(memblock + TempRecOffset + *iter));
					break;
				case TableMeta::FLOAT:
					(*TempTableCell)->fp = new float(CharToFloat(memblock + TempRecOffset + *iter));
					break;
				case TableMeta::CHAR:
					(*TempTableCell)->cp = new char[it->charNum];
					memcpy((*TempTableCell)->cp, memblock + TempRecOffset + *iter, it->charNum);
					(*TempTableCell)->charNum = it->charNum;
					break;
				}
				(*TempTableCell)->Next = TempTableRow->end;
				TempTableRow->tail = (*TempTableCell);
				TempTableCell = &((*TempTableCell)->Next);
			}
		}
		for (TableCell *ptr = TempTableRow->head; ptr != TempTableRow->end; ptr = ptr->Next) {
			switch (ptr->type) {
				case TableMeta::INT:
					cout << *(ptr->ip);
					break;
				case TableMeta::FLOAT:
					cout << *(ptr->fp);
					break;
				case TableMeta::CHAR:
					cout << ptr->cp;
					break;
			}
			cout << " | ";
		}
		cout << endl;
		Table.push_back(TempTableRow);
		TempRecOffset += RecLength;
	}
	cout << "-------------" << endl;
	//release
	delete[] memblock;
};

void BufferTable::Release() {
	--RefNum;
};

STATUS BufferTable::Push(TableRow *tableRow) {
	if (RecCount >= MaxRecCount) {
		return FULL;
	}
	else {
		++RecCount;
		Table.push_back(tableRow->DeepCopySelf());
		IsDirty = true;
		return SUCCESS;
	}
};

STATUS BufferTable::Del(uint32_t recOffset) {
	if (recOffset < HeadRecOffset
		|| recOffset > HeadRecOffset + RecCount
		|| Table[recOffset - HeadRecOffset]->IsEmpty) {
		return NOT_EXIST;
	} 
	else {
		Table[recOffset - HeadRecOffset]->IsEmpty = 1;
		IsDirty = true;
		return SUCCESS;
	}
};

STATUS BufferTable::Del(TableRow *tableRow) {
	tableRow->IsEmpty = 1;
	IsDirty = true;
	return SUCCESS;
};

BufferTable::~BufferTable() {//链表内存回收
	if(IsDirty)
		WriteBack();
	TableCell* TempTableCell;
	for (vector<TableRow*>::iterator iter = Table.begin(); iter != Table.end(); ++iter) {
		while (TempTableCell = (*iter)->head, TempTableCell != (*iter)->end) {
			(*iter)->head = (*iter)->head->Next;
			//delete
			switch (TempTableCell->type) {
			case TableMeta::INT:
				delete TempTableCell->ip;
				break;
			case TableMeta::FLOAT:
				delete TempTableCell->fp;
				break;
			case TableMeta::CHAR:
				delete[] TempTableCell->cp;
				break;
			}
			//monkey patch
			//if(TempTableCell->Next == (*iter)->end) {
			//	delete TempTableCell;
			//	break;
			//}
			delete TempTableCell;
		}
		delete *iter;
	}
};

STATUS BufferTable::WriteBack() {
	const char empty[] = "1";
	const char full[] = "0";
	const int FileSize = BLOCK_HEAD_OFFSET + RecCount * RecLength;

	char *memblock = new char[FileSize];
	uint32_t TempRecOffset;
	//write head
	memcpy(memblock, StringToChar(NextFileName, 32), 32);
	memcpy(memblock + 32, StringToChar(PrevFileName, 32), 32);
	memcpy(memblock + 64, IntToChar(RecCount), 4);
	memcpy(memblock + 68, IntToChar(HeadRecOffset), 4);
	//write reccord
	TempRecOffset = BLOCK_HEAD_OFFSET;
	for (vector<TableRow*>::iterator titer = Table.begin(); titer != Table.end(); ++titer) {
		if ((*titer)->IsEmpty) {
			//若为空
			memcpy(memblock + TempRecOffset, empty, 1);
			vector<uint32_t>::iterator iit = InnerOffsets.begin();
			for (vector<Attr>::iterator ait = attrs.begin(); ait != attrs.end() && iit != InnerOffsets.end(); ++ait, ++iit) {
				switch (ait->type) {//这个switch应该写成函数的但是现在懒
				case TableMeta::INT:
					memcpy(memblock + TempRecOffset + *iit, IntToChar((uint32_t)0), getSizeof(TableMeta::INT));
					break;
				case TableMeta::FLOAT:
					memcpy(memblock + TempRecOffset + *iit, FloatToChar(0.0f), getSizeof(TableMeta::FLOAT));
					break;
				case TableMeta::CHAR:
					memset(memblock + TempRecOffset + *iit, '\0', ait->charNum);
					break;
				default:
					break;
				}
			}
		}
		else {
			memcpy(memblock + TempRecOffset, full, 1);
			vector<uint32_t>::iterator iit = InnerOffsets.begin();
			for (TableCell *ptr = (*titer)->head; ptr != (*titer)->end && iit != InnerOffsets.end(); ptr = ptr->Next, ++iit) {
				switch (ptr->type) {
				case TableMeta::INT:
					memcpy(memblock + TempRecOffset + *iit, IntToChar(*(ptr->ip)), getSizeof(TableMeta::INT));
					break;
				case TableMeta::FLOAT:
					memcpy(memblock + TempRecOffset + *iit, FloatToChar(*(ptr->fp)), getSizeof(TableMeta::FLOAT));
					break;
				case TableMeta::CHAR:
					memcpy(memblock + TempRecOffset + *iit, ptr->cp, ptr->charNum);
					break;
				default:
					break;
				}
			}
		}
		TempRecOffset += RecLength;
	}
	ofstream file(FileName.data(), ios::out | ios::binary);
	if (file.is_open())
	{
		file.write(memblock, FileSize);
		file.close();

		cout << FileName << " : write complete." << endl;
		return SUCCESS;
	}
	else {
		cout << FileName << " : Unable to open file" << endl;
		return ERROR;
	}
};


BufferTable *BufferTable::ReadTable(string fileName, TableMeta tableMeta) {
	if (BuffList.find(fileName) == BuffList.end()) {
		//not found
		if (BuffList.size() >= 128) {
			for (BufferMap::iterator iter = BuffList.begin(); iter != BuffList.end(); ++iter) {
				if (iter->second->RefNum <= 0) {
					delete iter->second;
					BuffList.erase(iter->first);
				}
			}
		}
		BuffList[fileName] = new BufferTable(fileName, tableMeta);
	}
	++BuffList[fileName]->RefNum;
	return BuffList[fileName];
}

STATUS BufferTable::WriteBackAll() {
	for (BufferMap::iterator iter = BuffList.begin(); iter != BuffList.end(); ++iter) {
		if (iter->second->RefNum <= 0) {
			delete iter->second;
			BuffList.erase(iter->first);
		}
		else {
			return ERROR;
		}
	}
	return SUCCESS;
}