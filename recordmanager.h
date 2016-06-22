//
// Created by wfel on 2016/6/22.
//

#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include "TableMeta.h"

STATUS CreateTable(TableMeta tableMeta);
int InsertRecords(TableMeta tableMeta, TableRow *tableRow);
vector<TableRow*> GetRecords(TableMeta tableMeta, vector<int> attrIndex, ConditionNode *condition);
int DeleteRecords(TableMeta tableMeta, ConditionNode *condition);

#endif //MINISQL_RECORDMANAGER_H
