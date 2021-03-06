#pragma once
#include <vector>
#include <string>
#include "tablemeta.h"
#include "table.h"
#include "typeconvert.h"
#define BLOCK_SIZE_BYTE 4194304
#define BLOCK_HEAD_OFFSET 72

using namespace std;

enum STATUS {
	SUCCESS, ERROR, FULL, NOT_EXIST, EXIST
};

typedef map<string, BufferTable*> BufferMap;//这种释放策略坑

class BufferTable {
public:
	BufferTable(string fileName, TableMeta tableMeta);

	void Release();

	STATUS Push(TableRow *tableRow);

	STATUS Del(uint32_t offset);
	STATUS Del(TableRow *tableRow);

	~BufferTable();
	string FileName;
	string NextFileName;
	string PrevFileName;
	uint32_t RecCount;//条目数
	uint32_t MaxRecCount;
	uint32_t HeadRecOffset;//第几条
	uint32_t RecLength;//算入了第一个条目是否为空的标识符
	vector<Attr> attrs;
	vector<uint32_t> InnerOffsets;
	vector<TableRow*> Table;//表
	bool IsDirty;
	int RefNum;
	STATUS WriteBack();
	static BufferTable *ReadTable(string fileName, TableMeta tableMeta);
	static STATUS WriteBackAll();
private:
	static BufferMap BuffList;
};