//
// Created by kllilizxc on 2016/6/15.
//

#include <fstream>
#include "CatalogManager.h"
#include "error.h"

int CatalogManager::createTableMeta(string name) {
    int id = tableMetas.size(); //TODO or some other value
    TableMeta tableMeta(id);
    tableMetas.push_back(tableMeta);
    tableMap.insert(pair<string, int>(name, id));
    return id;
}

void CatalogManager::addAttrToTableMeta(int tableId, string name, Token::Type type, int charNum) {
    if(tableId >= tableMetas.size()) throw error("No such id in tableMetas!");
    TableMeta *tableMeta = findTableMetaFromId(tableId);
    TableMeta::ATTRTYPE attrtype;

    switch (type) {
        case Token::INT:
            attrtype = TableMeta::INT;
            break;
        case Token::FLOAT:
            attrtype = TableMeta::FLOAT;
            break;
        case Token::CHAR:
            attrtype = TableMeta::CHAR;
            break;
        default:
            throw error("INT or FLOAT or CHAR", type);
    }

    tableMeta->addAttr(name, attrtype, 0, charNum);
}

int CatalogManager::getTableIdFromName(string name) {
    return tableMap.find(name)->second;
}

TableMeta *CatalogManager::findTableMetaFromId(int id) {
    for (int i = 0; i < tableMetas.size(); ++i) {
        if(id == tableMetas[i].id) return &tableMetas[i];
    }
    throw error("No such table id!");
}

void CatalogManager::setTableAttrProperty(int tableId, string attrName, byte property) {
    TableMeta *tableMeta = findTableMetaFromId(tableId);

    Attr *attr = tableMeta->getAttrByName(attrName);
    attr->property |= property;
}

void CatalogManager::writeToFile() {
    ofstream file;
    file.open(fileName);
    if(!file.is_open()) throw error("can not open catalog file for write!");

    int tableSize = tableMetas.size();
    file << tableSize;
    for (int i = 0; i < tableSize; ++i) {
        TableMeta &table = tableMetas[i];
        file << " " << table.id << " " << getTableNameFromId(table.id);
        int attrSize = table.attrs.size();
        file << " " << attrSize;
        for (int j = 0; j < attrSize; ++j) {
            Attr &attr = table.attrs[j];
            file << " " << table.findAttrNameByIndex(j);
            file << " " << attr.type  << " " << attr.charNum << " " << attr.property;
        }
    }

    file.close();
}

string CatalogManager::getTableNameFromId(int id) {
    for (map<string, int>::const_iterator i = tableMap.begin(); i != tableMap.end(); ++i) {
        if(i->second == id) return i->first;
    }
    throw error("can't find table of id " + id);
}

void CatalogManager::readFromFile() {
    ifstream file;
    file.open(fileName);
    if(!file.is_open()) return;

    int tableSize;
    if(file.peek() == EOF, file.eof()) return;
    file >> tableSize;
    for (int i = 0; i < tableSize; ++i) {
        int tableId;
        string tableName;
        file >> tableId >> tableName;
        TableMeta table(tableId);
        tableMap[tableName] = tableId;
        int attrSize;
        file >> attrSize;
        for (int j = 0; j < attrSize; ++j) {
            string attrName;
            int type, charNum;
            byte property;
            file >> attrName >> type >> charNum >> property;
            Attr attr((TableMeta::ATTRTYPE)type, property, charNum);
            table.attrs.push_back(attr);
            table.attrMap[attrName] = j;
        }
        tableMetas.push_back(table);
    }

    file.close();
}

CatalogManager::CatalogManager() {
    readFromFile();
}

CatalogManager::~CatalogManager() {
    writeToFile();
}



















