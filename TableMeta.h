//
// Created by kllilizxc on 2016/6/15.
//

#ifndef MINISQL_TABLEMETA_H
#define MINISQL_TABLEMETA_H

#include <vector>
#include <string>
#include <map>

using namespace std;

typedef unsigned char byte;

class Attr;

class TableMeta {
public:
    map<string, int> attrMap;
    enum ATTRTYPE {
        INT, CHAR, FLOAT
    };
    int id;
    vector<Attr> attrs;

    string findAttrNameByIndex(int index);

    TableMeta(int tableId);

    void addAttr(string name, ATTRTYPE type, byte property, int charNum = 0);

    Attr *getAttrByName(string name);
};


struct Attr {
    enum PROPERTY : byte {
        UNIQUE = 1, PRIMARY = 2
    };
    TableMeta::ATTRTYPE type;
    int charNum;
    byte property;

    Attr(TableMeta::ATTRTYPE type, byte property, int charNum = 0);
};


#endif //MINISQL_TABLEMETA_H
