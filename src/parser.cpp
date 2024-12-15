#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include <map>
#include <cstring>
#include <iostream>
#define mp make_pair
using std :: string;
using std :: vector;
using std :: pair;

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Expr Syntax :: parse(Assoc &env) {
    ptr -> parse(env);
}

Expr Number :: parse(Assoc &env) {
    return Expr(new Fixnum(n));
}

Expr Identifier :: parse(Assoc &env) {}

Expr TrueSyntax :: parse(Assoc &env) {}

Expr FalseSyntax :: parse(Assoc &env) {}

//这里是对应输入中含有括号的情况
Expr List :: parse(Assoc &env) {
    List* list_ =dynamic_cast<List*>(this);
    int n = list_ -> stxs.size();
    if(n==0) {
        throw RuntimeError("");
    }
    SyntaxBase* operator_Syntax = (list_ -> stxs[0]).get();
    std::vector<Expr> content;

    if(Identifier* transform_Syntax = dynamic_cast<Identifier*>(operator_Syntax)) {
        std::string str = transform_Syntax -> s;
        if(str == "exit"){
            return Expr(new Exit());
        }else if(str == "+"|| str == "-" || str == "*") {
            if(n != 3) {    // 默认操作符之后只有两个数字
                throw RuntimeError("");
            }
            for(int i = 1;i < n; ++i) {
                content.push_back((list_ -> stxs[i]).get() ->parse(env));
            }
            
            if(str == "+") {
                return Expr(new Plus(content[0],content [1]));
            }else if (str == "-") {
                return Expr(new Minus(content[0],content [1]));
            }else if (str == "*") {
                return Expr(new Mult(content[0],content [1]));
            }
        }
        
    }else if (Number* transform_Syntax = dynamic_cast<Number*>(operator_Syntax)) {

    }
    

}

#endif