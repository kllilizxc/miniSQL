#include <iostream>
#include "interpreter.h"
#include "buffermanager.h"

using namespace std;

int main() {
    //char instr[] = "sadd create table 123) adwfa 12431.123 select  insertInto into 324.12 delete from create index && || > >= {} ad.sad";
    //char instr[] = "create table sad { a int, b float unique, c char(3), primary key(a) }";
    //char instr[] = "select a, b, c from sad where !(b < 3.0 || b < 2.0) && a > 5 || c < 'frog'";
    char instr[] = "insert into sad values(123, 23.21, 'asc' )";
    //char instr[] = "delete from A where";
    //char instr[] = "create index I on A (a, b, c)";

//    Lexer lexer(instr);
//
//    Token t = lexer.getNextToken();
//    while(t.type != Token::ERROR && t.type != Token::EOI) {
//        cout << t.type;
//        switch (t.type) {
//            case Token::INT:
//                cout << " : " << t.Int();
//                break;
//            case Token::FLOAT:
//                cout << " : " << t.Double();
//                break;
//            case Token::ID:
//                cout << " : " << t.String();
//                break;
//            default:
//                break;
//        }
//        cout << endl;
//        t = lexer.getNextToken();
//    }

    Interpreter *interpreter = new Interpreter(instr);
    interpreter->interpret();
    BufferTable::WriteBackAll();
    delete interpreter;
}