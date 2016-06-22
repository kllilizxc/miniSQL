//
// Created by kllilizxc on 2016/6/21.
//

#ifndef MINISQL_TABLE_H
#define MINISQL_TABLE_H

#include "TableMeta.h"
class TableCell {
private:
    union {
        uint32_t *ip;
        char *cp;
        double *dp;
    };
public:
    TableMeta::ATTRTYPE type;
    int charNum;
    TableCell *next;
    TableCell(const int value);
    TableCell(const double value);
    TableCell(const char *value, int charNum);

    int Int();
    double Double();
    char *Char();
};

class TableRow {
public:
    char isEmpty;
    TableCell *head;
    TableCell *tail;
    TableRow();
    void append(TableCell *cell);
};

#endif //MINISQL_TABLE_H
