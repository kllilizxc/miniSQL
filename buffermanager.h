#pragma once
#include <vector>
#include <string>
#include "tablemeta.h"
#include "table.h"
#include "typeconvert.h"

enum STATUS {
	SUCCESS, ERROR, FULL, NOT_EXIST, EXIST
};

class BufferTable {
public:
	typedef std::map<string, BufferTable*> BufferMap;//这种释放策略坑
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
	std::vector<Attr> attrs;
	std::vector<uint32_t> InnerOffsets;
	std::vector<TableRow*> Table;//表
	bool IsDirty;
	int RefNum;
	STATUS WriteBack();
	static BufferTable *ReadTable(string fileName, TableMeta tableMeta);
	static STATUS WriteBackAll();
private:
	static BufferMap BuffList;
};