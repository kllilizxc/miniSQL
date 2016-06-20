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
    const string fileName = "C:\\Users\\kllilizxc\\OneDrive\\Documents\\ClionProjects\\miniSQL\\catalog.data";
    vector<TableMeta> tableMetas;
    map<string, int> tableMap;
    int getTableIdFromName(string name);
    string getTableNameFromId(int id);
    TableMeta *findTableMetaFromId(int id);
    void writeToFile();
    void readFromFile();
public:
    CatalogManager();
    ~CatalogManager();
    int createTableMeta(string name);
    void addAttrToTableMeta(int tableId, string name, TokenType type, int charNum = 0);
    void setTableAttrProperty(int tableId, string attrName, byte property);
};


#endif //MINISQL_CATALOGMANAGER_H
