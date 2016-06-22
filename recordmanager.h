//
// Created by wfel on 2016/6/22.
//
#pragma once

#include "TableMeta.h"
#include "buffermanager.h"
#include "ConditionNode.h"

class RecordManager {
public:
    static STATUS CreateTable(TableMeta tableMeta);
    static int InsertRecords(TableMeta tableMeta, TableRow *tableRow);
    static vector<TableRow*> GetRecords(TableMeta tableMeta, vector<int> attrIndex, ConditionNode *condition);
    static int DeleteRecords(TableMeta tableMeta, ConditionNode *condition);
};
