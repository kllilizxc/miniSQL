//
// Created by kllilizxc on 2016/6/10.
//

#include "error.h"
#include <iostream>

using namespace std;

error::error(const char *expt, const char *get) {
    cout << endl << "ERROR! Expect " << expt << ", get " << get << " instead." << endl;
}

error::error(const char *expt, const TokenType get) {
    cout << endl << "ERROR! Expect " << expt << ", get Token type " << get << " instead." << endl;
}

error::error(const TokenType expt, const TokenType get) {
    cout << endl << "ERROR! Expect Token type " << expt << ", get Token type " << get << " instead." << endl;
}

error::error(const char *msg) {
    cout << endl << msg << endl;
}





