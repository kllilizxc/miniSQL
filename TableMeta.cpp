//
// Created by kllilizxc on 2016/6/15.
//

#include "TableMeta.h"
#include "error.h"

TableMeta::TableMeta(int tableId) : id(tableId) { }

void TableMeta::addAttr(string name, ATTRTYPE type, byte property, int charNum) {
    int index = attrs.size();
    attrs.push_back(Attr(type, property, charNum));
    attrMap.insert(pair<string, int>(name, index));
}

Attr *TableMeta::getAttrByName(string name) {
    return &attrs[attrMap.find(name)->second];
}

string TableMeta::findAttrNameByIndex(int index) {
    for (map<string, int>::const_iterator i = attrMap.begin(); i != attrMap.end(); ++i) {
        if(i->second == index) return i->first;
    }
    throw error("can't find attribute of index " + id);
}

TableMeta::TableMeta(const TableMeta &tableMeta) : attrMap(tableMeta.attrMap), id(tableMeta.id), attrs(tableMeta.attrs) { }


Attr::Attr(TableMeta::ATTRTYPE type, byte property, int charNum) : type(type), property(property), charNum(charNum) { }

