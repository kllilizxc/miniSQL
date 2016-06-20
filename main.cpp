#include <iostream>
#include "interpreter.h"

using namespace std;

int main() {
    //char instr[] = "sadd create table 123) adwfa 12431.123 select  insertInto into 324.12 delete from create index && || > >= {} ad.sad";
    char instr[] = "create table sad { a int, b float unique, c char(3), primary key(a) }";
    //char instr[] = "select a, b, c from A, B, C where a > 1 && b < 3 || c < 2";
    //char instr[] = "insert into A values(123, 23.21, 'asc' )";
    //char instr[] = "delete from A where";
    //char instr[] = "create index I on A (a, b, c)";

//    Lexer lexer(instr);
//
//    const Token *t = lexer.getNextToken();
//    while(t->type != ERROR && t->type != EOF) {
//        cout << t->type;
//        const IntToken *temp = dynamic_cast<const IntToken *>(t);
//        if(temp) cout << " : " << temp->value;
//        const FloatToken *temp2 = dynamic_cast<const FloatToken *>(t);
//        if(temp2) cout << " : " << temp2->value;
//        const StringToken *temp3 = dynamic_cast<const StringToken *>(t);
//        if(temp3) cout << " : " << temp3->value;
//        cout << endl;
//        t = lexer.getNextToken();
//    }

    Interpreter *interpreter = new Interpreter(instr);
    interpreter->interpret();
    delete interpreter;
}