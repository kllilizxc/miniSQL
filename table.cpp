//
// Created by kllilizxc on 2016/6/21.
//

#include <cstring>
#include "table.h"

TableCell::TableCell(int value) : type(TableMeta::INT), ip(new uint32_t(value)) { }

TableCell::TableCell(const double value) : type(TableMeta::DOUBLE), dp(new double(value)) { }

TableCell::TableCell(const char *value, int charNum) : type(TableMeta::CHAR), charNum(charNum), cp(new char[charNum + 1]) {
    strcpy(cp, value);
}

int TableCell::Int() {
    return *ip;
}

double TableCell::Double() {
    return *dp;
}

char *TableCell::Char() {
    return cp;
}


TableRow::TableRow() : isEmpty(0), head(new TableCell(0)), tail(head) {
    tail->next = NULL;
}

void TableRow::append(TableCell *cell) {
    tail->next = cell;
    tail = cell;
    tail->next = NULL;
}



