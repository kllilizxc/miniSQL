#include <iostream>
#include <fstream>
#include <map>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include "buffermanager.h"
#include "typeconvert.h"
#include "conditionnode.h"
#include "comparevalue.h"
#include "recordmanager.h"

const string DB_FILE_NAME = "test_db.dbi";
const string DB_NAME = "test_db";

map<string, DbInfo*> RecordManager::DbInfos;

char *ConstructTableFileHead(string nextFileName, string prevFileName, uint32_t recCount, uint32_t headRecOffset) {
	char *memblock = new char[72];
	//write head
	memcpy(memblock, StringToChar(nextFileName, 32), 32);
	memcpy(memblock + 32, StringToChar(prevFileName, 32), 32);
	memcpy(memblock + 64, IntToChar(recCount), 4);
	memcpy(memblock + 68, IntToChar(headRecOffset), 4);
	return memblock;
}

DbInfo::DbInfo(char* memblock) {
	//char dbname[32]|uint32_t tablenum|uint32_t id|char HeadFileName[32]|id|HeadFileName 写入 <dbname>.dbi (db info)
	RefNum = 0;
	const uint32_t HeadOffset = 36;
	const uint32_t RecLength = 36;

	DbInfoRec TempDbInfoRec;
	uint32_t TempRecOffset;

	DbName = string(memblock, memblock + 32);
	TableNum = CharToInt(memblock + 32);
	TempRecOffset = HeadOffset;
	for (uint32_t i = 0; i < TableNum; ++i, TempRecOffset += RecLength) {
		TempDbInfoRec.id = CharToInt(memblock + HeadOffset);
		TempDbInfoRec.HeadFileName = string(memblock + HeadOffset + 4, memblock + HeadOffset + RecLength);
		DbInfoRecs[TempDbInfoRec.id] = TempDbInfoRec;
	}
}

char *DbInfo::ConvertToMemblock(size_t *retPptr) {
	const uint32_t HeadOffset = 36;
	const uint32_t RecLength = 36;
	size_t size = HeadOffset + RecLength * TableNum;
	*retPptr = size;

	char* memblock;
	memblock = new char[size];
	memcpy(memblock, StringToChar(DbName, 32), 32);
	memcpy(memblock + 32, IntToChar(TableNum), 4);

	uint32_t TempRecOffset = HeadOffset;
	map<uint32_t, DbInfoRec>::iterator iter = DbInfoRecs.begin();
	for (uint32_t i = 0; i < TableNum; ++i, ++iter, TempRecOffset += RecLength) {
		memcpy(memblock + TempRecOffset, IntToChar(iter->second.id), 4);
		memcpy(memblock + TempRecOffset + 4, StringToChar(iter->second.HeadFileName, 32), 32);
	}

	return memblock;
}

STATUS CreateDbInfo(string DB_NAME, string DB_FILE_Name) {
	//cout << "open failed, create one" << DB_FILE_Name << endl;

	char memblock[36];
	memcpy(memblock, StringToChar(DB_NAME, 32), 32);
	memcpy(memblock + 32, IntToChar((uint32_t)0), 4);

	ofstream file(DB_FILE_Name.data(), ios::out | ios::binary);
	if (file.is_open())
	{
		file.write(memblock, 36);
		file.close();

		//cout << DB_FILE_Name << " : create complete." << endl;
		return SUCCESS;
	}
	else {
		//cout << DB_FILE_Name << " : Unable to open file" << endl;
		return ERROR;
	}
}

STATUS CreateTableFile(string fileName, string nextFileName, string prevFileName, uint32_t recCount, uint32_t headRecOffset) {
	//char FileName[32]下一块文件名|string上一块文件名|uint32_t 记录数|int起始条目偏移|(offset: 72bytes)char是否为空数据|数据|char是否为空数据|数据| -> <dbname>.<tableid(16进制)>.dbt.000 (db table)
	ofstream TableFile(fileName.data(), ios::out | ios::binary);
	if (TableFile.is_open())
	{
		TableFile.write(ConstructTableFileHead(nextFileName, prevFileName, recCount, headRecOffset), 72);
		TableFile.close();

		//cout << fileName << " : create complete." << endl;
	}
	else {
		//cout << fileName << " : Unable to open file" << endl;
		return ERROR;
	}
}

DbInfo *RecordManager::GetDbInfo(string dbName, string dbFileName) {//懒得写引用计数
	if (DbInfos.find(dbFileName) == DbInfos.end()) {
		//not found
		if (DbInfos.size() >= 128) {
			for (map<string, DbInfo*>::iterator iter = DbInfos.begin(); iter != DbInfos.end(); ++iter) {
				if(iter->second->RefNum <= 0) {
					delete iter->second;
					DbInfos.erase(iter->first);
				}
			}
		}

		//open file
		streampos size;
		char *memblock;
		ifstream DbInfoFile(dbFileName.data(), ios::in | ios::binary | ios::ate);
		if (!DbInfoFile.is_open()) {
			//cout << "open failed. try to create." << endl;
			if(CreateDbInfo(dbName, dbFileName) == ERROR)
				return NULL;
			DbInfoFile.open(dbFileName.data(), ios::in | ios::binary | ios::ate);
		}

		if (DbInfoFile.is_open()) {
			size = DbInfoFile.tellg();
			memblock = new char[size];
			DbInfoFile.seekg(0, ios::beg);
			DbInfoFile.read(memblock, size);
			DbInfoFile.close();
			//cout << "opened dbinfo" << DB_FILE_NAME << endl;
		}
		else {
			//cout << "open failed due to unknow." << endl;
			return NULL;
		}
		//process
		DbInfos[dbFileName] = new DbInfo(memblock);
		delete[] memblock;
	}
	DbInfos[dbFileName]->RefNum++;
	return DbInfos[dbFileName];
}

void DbInfo::WriteBack() {
	//write back to db info
	char *memblock = NULL;
	size_t *pptrs = new size_t();
	memblock = ConvertToMemblock(pptrs);
	ofstream file(DB_FILE_NAME.data(), ios::out | ios::binary);
	if (file.is_open())
	{
		file.write(memblock, *pptrs);
		file.close();
		delete pptrs;
		delete[] memblock;

		//cout << "the entire file content is written to disk." << endl;
	}
	else {
		//cout << "Unable to open file" << endl;
	}
};

void DbInfo::Release() {
	RefNum--;
}

DbInfo::~DbInfo() {
	WriteBack();
}

void RecordManager::WriteBackAll() {
	for (map<string, DbInfo*>::iterator iter = DbInfos.begin(); iter != DbInfos.end(); ++iter) {
		if (iter->second->RefNum <= 0) {
			delete iter->second;
			DbInfos.erase(iter->first);
		}
	}
};

STATUS RecordManager::CreateTable(TableMeta *tableMeta) {
	//process
	DbInfo *Dbinfo = GetDbInfo(DB_NAME, DB_FILE_NAME);
	//construct new record
	DbInfoRec TempDbInfoRec;
	char IdBuff[9];
	if (Dbinfo->DbInfoRecs.find(tableMeta->id) != Dbinfo->DbInfoRecs.end()) {
		//cout << "this table exist" << endl;
		return EXIST;
	}
	TempDbInfoRec.id = tableMeta->id;
	snprintf(IdBuff, sizeof(IdBuff), "%x", tableMeta->id);
	TempDbInfoRec.HeadFileName = DB_NAME + string(".") + string(IdBuff) + string(".dbt.000");
	Dbinfo->DbInfoRecs[tableMeta->id] = TempDbInfoRec;
	Dbinfo->TableNum++;
	//create table file
	CreateTableFile(TempDbInfoRec.HeadFileName, "", "", 0, 0);
	Dbinfo->Release();
	return SUCCESS;
}

char *IncreaseLastNumbers(const char *str, int size) {
	char *c = new char[size];
	char *pend;
	strcpy(c, str + (strlen(str) - size));
	pend = c + (size - 1);
	while (1) {
		*pend++;
		if (*pend > '9') {
			*pend = '0';
		}
		else {
			break;
		}
		if (pend == c)
			break;
		pend--;
	}
	return c;
}

int RecordManager::InsertRecords(TableMeta *tableMeta, TableRow *tableRow) {
	//open file
	DbInfo *Dbinfo = GetDbInfo(DB_NAME, DB_FILE_NAME);
	//not found
	if (Dbinfo->DbInfoRecs.find(tableMeta->id) == Dbinfo->DbInfoRecs.end()) {
		return 0;
	}
	string TableFileName = Dbinfo->DbInfoRecs[tableMeta->id].HeadFileName;
	BufferTable *InsertableTable;
	string TempTableFileName;
	while(1) {//如果在dbi中保持一个文件尾指针会更快但是懒。。。
		InsertableTable = BufferTable::ReadTable(TableFileName, tableMeta);
		if (InsertableTable->RecCount < InsertableTable->MaxRecCount) {
			//此文件未满
			break;
		}
		else {
			//文件已满
			TableFileName = InsertableTable->NextFileName;
			if (!strlen(TableFileName.data())) {//下一文件为空
				char IdBuff[9];
				snprintf(IdBuff, sizeof(IdBuff), "%x", tableMeta->id);
				TempTableFileName = DB_NAME + string(".") + string(IdBuff) + string(".dbt.") + IncreaseLastNumbers(InsertableTable->NextFileName.data(), 3);
				//这里要手动改脏数据不优雅
				InsertableTable->NextFileName = TempTableFileName;
				InsertableTable->IsDirty = 1;
				//create table file
				CreateTableFile(TempTableFileName, "", InsertableTable->FileName, 0, InsertableTable->HeadRecOffset + InsertableTable->RecCount);
			}
			//释放旧文件
			InsertableTable->Release();
		}
	}
	InsertableTable->Push(tableRow);
	InsertableTable->Release();
	Dbinfo->Release();
	return 1;
}

bool HandleCondtion(TableRow* tableRow, ConditionNode *condition) {
	//cout << "now : " << condition->type << endl;
	if (condition->left->type != ConditionNode::ATTR_INT
		&& condition->left->type != ConditionNode::ATTR_FLOAT
		&& condition->left->type != ConditionNode::ATTR_CHAR
		&& condition->left->type != ConditionNode::INT
		&& condition->left->type != ConditionNode::FLOAT
		&& condition->left->type != ConditionNode::CHAR) {
		//此节点为逻辑运算符节点
		if(condition->type == ConditionNode::NOT) {
			return HandleCondtion(tableRow, condition->left);
		}
		bool LeftResult = false, RightResult = false;
		//cout << "left : " << condition->left->type << endl;
		LeftResult = HandleCondtion(tableRow, condition->left);
		//cout << "right : " << condition->right->type << endl;
		RightResult = HandleCondtion(tableRow, condition->right);
		switch (condition->type) {
		case ConditionNode::AND:
			return LeftResult && RightResult;
			break;
		case ConditionNode::OR:
			return LeftResult || RightResult;
			break;
		default:
			return false;
		}
	}
	else {
		//此节点为算数运算节点
		//类型转换烦。。。
		ConditionNode::conType AType;

		int LeftI;
		float LeftF;
		string LeftS;

		int RightI;
		float RightF;
		string RightS;
		//转换左边
		//cout << "convertleft" << endl;
		switch (condition->left->type)
		{
		case ConditionNode::ATTR_INT:
		case ConditionNode::ATTR_FLOAT:
		case ConditionNode::ATTR_CHAR:
			switch (tableRow->at(condition->left->getAttrValue())->type) {
			case TableMeta::INT:
				LeftI = *(tableRow->at(condition->left->getAttrValue())->ip);
				AType = ConditionNode::INT;
				break;
			case TableMeta::FLOAT:
				LeftF = *(tableRow->at(condition->left->getAttrValue())->fp);
				AType = ConditionNode::FLOAT;
				break;
			case TableMeta::CHAR:
				LeftS = string(tableRow->at(condition->left->getAttrValue())->cp);
				AType = ConditionNode::CHAR;
				break;
			}
			break;
		case ConditionNode::INT:
			LeftI = condition->left->getIntValue();
			AType = ConditionNode::INT;
			break;
		case ConditionNode::FLOAT:
			LeftF = condition->left->getFloatValue();
			AType = ConditionNode::FLOAT;
			break;
		case ConditionNode::CHAR:
			LeftS = condition->left->getCharValue();
			AType = ConditionNode::CHAR;
			break;
		default:
			break;
		}
		//右边
		//cout << "convertright" << endl;
		switch (condition->right->type)
		{
		case ConditionNode::ATTR_INT:
		case ConditionNode::ATTR_FLOAT:
		case ConditionNode::ATTR_CHAR:
			switch (tableRow->at(condition->right->getAttrValue())->type) {
			case TableMeta::INT:
				RightI = *(tableRow->at(condition->right->getAttrValue())->ip);
				return CompareValue(LeftI, RightI, condition->type);
				break;
			case TableMeta::FLOAT:
				RightF = *(tableRow->at(condition->right->getAttrValue())->fp);
				return CompareValue(LeftF, RightF, condition->type);
				break;
			case TableMeta::CHAR:
				RightS = string(tableRow->at(condition->right->getAttrValue())->cp);
				return CompareValue(LeftS, RightS, condition->type);
				break;
			}
			break;
		case ConditionNode::INT:
			//cout << "attr node" << endl;
			RightI = condition->right->getIntValue();
			return CompareValue(LeftI, RightI, condition->type);
			break;
		case ConditionNode::FLOAT:
			RightF = condition->right->getFloatValue();
			return CompareValue(LeftF, RightF, condition->type);
			break;
		case ConditionNode::CHAR:
			RightS = condition->right->getCharValue();
			return CompareValue(LeftS, RightS, condition->type);
			break;
		default:
			break;
		}
	}
}

vector<TableRow*> RecordManager::GetRecords(TableMeta *tableMeta, vector<int> attrIndex, ConditionNode *condition) {
	vector<TableRow*> Finale;
	//这还不能直接push指针，还得复制内存woc(深拷贝)
	//先不写index
	//open file是复制insert的，不优雅
	DbInfo *Dbinfo = GetDbInfo(DB_NAME, DB_FILE_NAME);
	//not found
	if (Dbinfo->DbInfoRecs.find(tableMeta->id) == Dbinfo->DbInfoRecs.end()) {
		return Finale;
	}
	//no index
	string TableFileName = Dbinfo->DbInfoRecs[tableMeta->id].HeadFileName;
	BufferTable *TempTable;
	string TempTableFileName;
	while (strlen(TableFileName.data())) {
		//cout << TableFileName << endl;
		TempTable = BufferTable::ReadTable(TableFileName, tableMeta);
		for (vector<TableRow*>::iterator iter = TempTable->Table.begin(); iter != TempTable->Table.end(); ++iter) {
			if (!(*iter)->IsEmpty && (!condition || HandleCondtion(*iter, condition))) {
				Finale.push_back((*iter)->Filter(attrIndex));
				//cout << "find one !" << endl;
			}
		}
		TableFileName = TempTable->NextFileName;
		TempTable->Release();
	}
	Dbinfo->Release();
	return Finale;
}

int RecordManager::DeleteRecords(TableMeta *tableMeta, ConditionNode *condition) {
	int DelRecords = 0;
	DbInfo *Dbinfo = GetDbInfo(DB_NAME, DB_FILE_NAME);
	//not found
	if (Dbinfo->DbInfoRecs.find(tableMeta->id) == Dbinfo->DbInfoRecs.end()) {
		return 0;
	}
	//no index
	string TableFileName = Dbinfo->DbInfoRecs[tableMeta->id].HeadFileName;
	BufferTable *TempTable;
	string TempTableFileName;
	while (strlen(TableFileName.data())) {
		TempTable = BufferTable::ReadTable(TableFileName, tableMeta);
		for (vector<TableRow*>::iterator iter = TempTable->Table.begin(); iter != TempTable->Table.end(); ++iter) {
			if (!(*iter)->IsEmpty && (!condition || HandleCondtion(*iter, condition))) {
				if(TempTable->Del(*iter) == SUCCESS)
					DelRecords++;
			}
		}
		TableFileName = TempTable->NextFileName;
		TempTable->Release();
	}
	Dbinfo->Release();
	return DelRecords;
}