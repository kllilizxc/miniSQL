//
// Created by kllilizxc on 2016/6/15.
//

#ifndef MINISQL_CATALOGMANAGER_H
#define MINISQL_CATALOGMANAGER_H

#include "TableMeta.h"
#include "token.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

class CatalogManager {
private:
    const string fileName = "catalog.data";
    vector<TableMeta> tableMetas;
    map<string, int> tableMap;
    void writeToFile();
    void readFromFile();
public:
    CatalogManager();
    ~CatalogManager();
    TableMeta *findTableMetaFromId(int id);
    string getTableNameFromId(int id);
    int getTableIdFromName(string name);
    int createTableMeta(string name);
    void addAttrToTableMeta(int tableId, string name, Token::Type type, int charNum = 0);
    void setTableAttrProperty(int tableId, string attrName, byte property);
};


#endif //MINISQL_CATALOGMANAGER_H
