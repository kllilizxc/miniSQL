#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "buffermanager.h"
#include "typeconvert.h"
#include "conditionnode.h"
#include "comparevalue.h"

const string DB_FILE_NAME = "test_db.dbi";
const string DB_NAME = "test_db";

TableRow* DeepCopyTableRow(TableRow* tableRow) {

}

char *ConstructTableFileHead(string nextFileName, string prevFileName, uint32_t recCount, uint32_t headRecOffset) {
	char *memblock = new char[72];
	//write head
	memcpy(memblock, StringToChar(nextFileName, 32), 32);
	memcpy(memblock + 32, StringToChar(prevFileName, 32), 32);
	memcpy(memblock + 64, IntToChar(recCount), 4);
	memcpy(memblock + 68, IntToChar(headRecOffset), 4);
}

struct DbInfoRec {
	uint32_t id;
	string HeadFileName;
};

class DbInfo {
public:
	DbInfo(char* memblock);
	string DbName;
	uint32_t TableNum;
	map<uint32_t, DbInfoRec> DbInfoRecs;
	char * ConvertToMemblock();
};

DbInfo::DbInfo(char* memblock) {
	//char dbname[32]|uint32_t tablenum|uint32_t id|char HeadFileName[32]|id|HeadFileName 写入 <dbname>.dbi (db info)
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

char *DbInfo::ConvertToMemblock() {
	const uint32_t HeadOffset = 36;
	const uint32_t RecLength = 36;

	char* memblock;
	memblock = new char[HeadOffset + RecLength * TableNum];
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
	cout << "open failed, create one" << DB_FILE_Name << endl;

	char memblock[36];
	memcpy(memblock, StringToChar(DB_NAME, 32), 32);
	memcpy(memblock + 32, IntToChar((uint32_t)0), 4);

	ofstream file(DB_FILE_Name.data(), ios::out | ios::binary);
	if (file.is_open())
	{
		file.write(memblock, 36);
		file.close();

		cout << DB_FILE_Name << " : create complete." << endl;
		return SUCCESS;
	}
	else {
		cout << DB_FILE_Name << " : Unable to open file" << endl;
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

		cout << fileName << " : create complete." << endl;
	}
	else {
		cout << fileName << " : Unable to open file" << endl;
		return ERROR;
	}
}

STATUS CreateTable(TableMeta tableMeta) {
	//open file
	streampos size;
	char *memblock;
	ifstream DbInfoFile(DB_FILE_NAME.data(), ios::in | ios::binary | ios::ate);
	if (!DbInfoFile.is_open()) {
		cout << "open failed. try to create." << endl;
		if(CreateDbInfo(DB_NAME, DB_FILE_NAME) == ERROR)
			return ERROR;
		DbInfoFile.open(DB_FILE_NAME.data(), ios::in | ios::binary | ios::ate);
	}

	if (DbInfoFile.is_open()) {
		size = DbInfoFile.tellg();
		memblock = new char[size];
		DbInfoFile.seekg(0, ios::beg);
		DbInfoFile.read(memblock, size);
		DbInfoFile.close();
		cout << "opened dbinfo" << DB_FILE_NAME << endl;
	}
	else {
		cout << "open failed due to unknow." << endl;
		return ERROR;
	}
	//process
	DbInfo Dbinfo(memblock);
	delete[] memblock;
	//construct new record
	DbInfoRec TempDbInfoRec;
	char IdBuff[9];
	if (Dbinfo.DbInfoRecs.find(tableMeta.id) != Dbinfo.DbInfoRecs.end()) {
		return EXIST;
	}
	TempDbInfoRec.id = tableMeta.id;
	itoa(tableMeta.id, IdBuff, 16);
	TempDbInfoRec.HeadFileName = DB_NAME + string(".") + string(IdBuff) + string(".dbt.000");
	Dbinfo.DbInfoRecs[tableMeta.id] = TempDbInfoRec;
	Dbinfo.TableNum++;
	//create table file
	CreateTableFile(TempDbInfoRec.HeadFileName, "", "", 0, 0);
	//write back to db info
	memblock = Dbinfo.ConvertToMemblock();
	ofstream file(TempDbInfoRec.HeadFileName.data(), ios::out | ios::binary);
	if (file.is_open())
	{
		file.write(memblock, sizeof(memblock));
		file.close();
		delete[] memblock;

		cout << "the entire file content is written to disk." << endl;
		return SUCCESS;
	}
	else { 
		cout << "Unable to open file" << endl;
		return ERROR;
	}
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

int InsertRecords(TableMeta tableMeta, TableRow *tableRow) {
	//open file
	streampos size;
	char *memblock;
	ifstream DbInfoFile(DB_FILE_NAME.data(), ios::in | ios::binary | ios::ate);
	if (DbInfoFile.is_open()) {
		size = DbInfoFile.tellg();
		memblock = new char[size];
		DbInfoFile.seekg(0, ios::beg);
		DbInfoFile.read(memblock, size);
		DbInfoFile.close();
		cout << "opened dbinfo" << DB_FILE_NAME << endl;
	}
	else {
		cout << "open failed due to unknow." << endl;
		return ERROR;
	}
	DbInfo Dbinfo(memblock);
	delete[] memblock;
	//not found
	if (Dbinfo.DbInfoRecs.find(tableMeta.id) == Dbinfo.DbInfoRecs.end()) {
		return 0;
	}
	string TableFileName = Dbinfo.DbInfoRecs[tableMeta.id].HeadFileName;
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
				itoa(tableMeta.id, IdBuff, 16);
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
	return 1;
}

bool HandleCondtion(TableRow* tableRow, ConditionNode *condition) {
	if (condition->left->type != ConditionNode::ATTR
		&& condition->left->type != ConditionNode::INT
		&& condition->left->type != ConditionNode::FLOAT
		&& condition->left->type != ConditionNode::CHAR) {
		//此节点为逻辑运算符节点
		bool LeftResult = false, RightResult = false;
		LeftResult = HandleCondtion(tableRow, condition->left);
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
		switch (condition->left->type)
		{
		case ConditionNode::ATTR:
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
			LeftS = condition->left->getStringValue();
			AType = ConditionNode::CHAR;
			break;
		default:
			break;
		}
		//右边
		switch (condition->right->type)
		{
		case ConditionNode::ATTR:
			switch (tableRow->at(condition->right->getAttrValue())->type) {
			case TableMeta::INT:
				RightI = *(tableRow->at(condition->right->getAttrValue())->ip);
				return CompareValue(LeftI, RightI, AType);
				break;
			case TableMeta::FLOAT:
				RightF = *(tableRow->at(condition->right->getAttrValue())->fp);
				return CompareValue(LeftF, RightF, AType);
				break;
			case TableMeta::CHAR:
				RightS = string(tableRow->at(condition->right->getAttrValue())->cp);
				return CompareValue(LeftS, RightS, AType);
				break;
			}
			break;
		case ConditionNode::INT:
			RightI = condition->right->getIntValue();
			return CompareValue(LeftI, RightI, AType);
			break;
		case ConditionNode::FLOAT:
			RightF = condition->right->getFloatValue();
			return CompareValue(LeftF, RightF, AType);
			break;
		case ConditionNode::CHAR:
			RightS = condition->right->getStringValue();
			return CompareValue(LeftS, RightS, AType);
			break;
		default:
			break;
		}
	}
}

vector<TableRow*> GetRecords(TableMeta tableMeta, vector<int> attrIndex, ConditionNode *condition) {
	vector<TableRow*> Finale;
	//这还不能直接push指针，还得复制内存woc(深拷贝)
	//先不写index
	//open file是复制insert的，不优雅
	streampos size;
	char *memblock;
	ifstream DbInfoFile(DB_FILE_NAME.data(), ios::in | ios::binary | ios::ate);
	if (DbInfoFile.is_open()) {
		size = DbInfoFile.tellg();
		memblock = new char[size];
		DbInfoFile.seekg(0, ios::beg);
		DbInfoFile.read(memblock, size);
		DbInfoFile.close();
		cout << "opened dbinfo" << DB_FILE_NAME << endl;
	}
	else {
		cout << "open failed due to unknow." << endl;
		return Finale;
	}
	DbInfo Dbinfo(memblock);
	delete[] memblock;
	//not found
	if (Dbinfo.DbInfoRecs.find(tableMeta.id) == Dbinfo.DbInfoRecs.end()) {
		return Finale;
	}
	//no index
	string TableFileName = Dbinfo.DbInfoRecs[tableMeta.id].HeadFileName;
	BufferTable *TempTable;
	string TempTableFileName;
	while (strlen(TableFileName.data())) {
		TempTable = BufferTable::ReadTable(TableFileName, tableMeta);
		for (vector<TableRow*>::iterator iter = TempTable->Table.begin(); iter != TempTable->Table.end(); ++iter) {
			if (HandleCondtion(*iter, condition))
				Finale.push_back((*iter)->DeepCopySelf());
		}
		TableFileName = TempTable->NextFileName;
		TempTable->Release();
	}
	return Finale;
}

int DeleteRecords(TableMeta tableMeta, ConditionNode *condition) {
	int DelRecords = 0;
	//一样是复制的。。。
	streampos size;
	char *memblock;
	ifstream DbInfoFile(DB_FILE_NAME.data(), ios::in | ios::binary | ios::ate);
	if (DbInfoFile.is_open()) {
		size = DbInfoFile.tellg();
		memblock = new char[size];
		DbInfoFile.seekg(0, ios::beg);
		DbInfoFile.read(memblock, size);
		DbInfoFile.close();
		cout << "opened dbinfo" << DB_FILE_NAME << endl;
	}
	else {
		cout << "open failed due to unknow." << endl;
		return 0;
	}
	DbInfo Dbinfo(memblock);
	delete[] memblock;
	//not found
	if (Dbinfo.DbInfoRecs.find(tableMeta.id) == Dbinfo.DbInfoRecs.end()) {
		return 0;
	}
	//no index
	string TableFileName = Dbinfo.DbInfoRecs[tableMeta.id].HeadFileName;
	BufferTable *TempTable;
	string TempTableFileName;
	while (strlen(TableFileName.data())) {
		TempTable = BufferTable::ReadTable(TableFileName, tableMeta);
		for (vector<TableRow*>::iterator iter = TempTable->Table.begin(); iter != TempTable->Table.end(); ++iter) {
			if (HandleCondtion(*iter, condition)) {
				if(TempTable->Del(*iter) == SUCCESS)
					DelRecords++;
			}
		}
		TableFileName = TempTable->NextFileName;
		TempTable->Release();
	}
	return DelRecords;
}