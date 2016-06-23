//
// Created by wfel on 2016/6/22.
//
#pragma once

#include "TableMeta.h"
#include "buffermanager.h"
#include "ConditionNode.h"

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
    char * ConvertToMemblock(size_t*);
};

class RecordManager {
public:
    static STATUS CreateTable(TableMeta tableMeta);
    static int InsertRecords(TableMeta tableMeta, TableRow *tableRow);
    static vector<TableRow*> GetRecords(TableMeta tableMeta, vector<int> attrIndex, ConditionNode *condition);
    static int DeleteRecords(TableMeta tableMeta, ConditionNode *condition);
    static map<string, DbInfo*> DbInfos;
    static DbInfo *GetDbInfo(string DbName);
};
