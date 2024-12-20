#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include <regex>
#include <cctype>
#include <utility>

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
    return ptr -> parse(env);//自动调用它指向的Syntaxbase
}

Expr Number :: parse(Assoc &env) {
    return Expr(new Fixnum(n));
}

//字符串 如果一些字符接近数字，会转化成整数
Expr Identifier :: parse(Assoc &env) {
    if(!std::isalnum(s.back())) {
        return Expr(new Var(s));
    }
    if (std::regex_match(s, std::regex("^[+-]?\\d*(\\.\\d+)?([eE][+-]?\\d+)?$"))) {
        return Expr(new Fixnum(std::stof(s)));
    }
    return Expr(new Var(s));
}

//真 无参 -> struct True
Expr TrueSyntax :: parse(Assoc &env) {
    return Expr(new True());
}

//假 无参 -> struct False
Expr FalseSyntax :: parse(Assoc &env) {
    return Expr(new False());
}

//这里是对应输入中含有括号的情况
Expr List :: parse(Assoc &env) {
    List* list_ =dynamic_cast<List*>(this);
    int n = list_ -> stxs.size();
    if(n==0) {
        return Expr(new Quote(Syntax(new List)));
    }
    SyntaxBase* operator_Syntax = (list_ -> stxs[0]).get();
    std::vector<Expr> content;

    if(Identifier* transform_Syntax = dynamic_cast<Identifier*>(operator_Syntax)) {
        std::string str = transform_Syntax -> s;
        
        if(primitives.count(str)) {
            Expr expr = operator_Syntax->parse(env);
            std::vector<Expr> rand;
            for(int i = 1;i < n;++i) {
                rand.push_back(list_->stxs[i].parse(env));
            }
            return Expr(new Apply(expr,rand));
        }


        if(str == "exit"){
            return Expr(new Exit());
        }else if(str == "+"|| str == "-" || str == "*") {
            if(n != 3) {    // 默认操作符之后只有两个数字
                throw RuntimeError("");
            }
            
            // if(str == "+") {
            //     return Expr(new Plus(content[0],content [1]));
            // }else if (str == "-") {
            //     return Expr(new Minus(content[0],content [1]));
            // }else if (str == "*") {
            //     return Expr(new Mult(content[0],content [1]));
            // }
        }else if(str == "quote") {
            if(n != 2) {    // 默认操作符之后只有1个成员
                throw RuntimeError("");
            }
            Syntax syntax_tmp = list_ -> stxs[1];
            return Expr(new Quote(syntax_tmp));
        }else if(str == "void") {
            return Expr(new MakeVoid());
        }else if(str == "cons") {
            if(n != 3) {    // 默认操作符之后只有两个数字
                throw RuntimeError("");
            }
            for(int i = 1;i < n; ++i) {
                content.push_back((list_ -> stxs[i]).get() ->parse(env));
            }
            return Expr(new Cons(content[0],content[1]));
        }else if(str == "cdr") {
            if(n != 2) {    
                throw RuntimeError("");
            }
            if(List* list_1 =dynamic_cast<List*>(list_->stxs[1].get())) {
                return Expr(new Cdr(list_1->parse(env)));
            }
        }else if(str == "car") {
            if(n != 2) {    // 默认操作符之后只有1个成员
                throw RuntimeError("");
            }
            if(List* list_1 =dynamic_cast<List*>(list_->stxs[1].get())) {
                return Expr(new Car(list_1->parse(env)));
            }
        }else if(str == "begin") {
            for(int i = 1;i < n;++i) {
                content.push_back(list_ -> stxs[i].parse(env));
            }
            return Expr(new Begin(content));
        }else if(str == "if") {
            if(n != 4) {
                throw RuntimeError("");
            }
            for(int i = 1;i < n;++i) {
                content.push_back(list_ -> stxs[i].parse(env));
            }
            return Expr(new If(content[0],content[1],content[2]));
        }else if(str == "<") {
            if(n != 3) {
                throw RuntimeError("");
            }
            for(int i = 1;i < n;++i) {
                content.push_back(list_ -> stxs[i].parse(env));
                if(content.back()->e_type != E_FIXNUM) {
                    throw RuntimeError("");
                }
            }
            return Expr(new Less(content[0],content[1]));
        }else if(str == "<=") {
            if(n != 3) {
                throw RuntimeError("");
            }
            for(int i = 1;i < n;++i) {
                content.push_back(list_ -> stxs[i].parse(env));
                if(content.back()->e_type != E_FIXNUM) {
                    throw RuntimeError("");
                }
            }
            return Expr(new LessEq(content[0],content[1]));
        }else if(str == "=") {
            if(n != 3) {
                throw RuntimeError("");
            }
            for(int i = 1;i < n;++i) {
                content.push_back(list_ -> stxs[i].parse(env));
                if(content.back()->e_type != E_FIXNUM) {
                    throw RuntimeError("");
                }
            }
            return Expr(new Equal(content[0],content[1]));
        }else if(str == ">=") {
            if(n != 3) {
                throw RuntimeError("");
            }
            for(int i = 1;i < n;++i) {
                content.push_back(list_ -> stxs[i].parse(env));
                if(content.back()->e_type != E_FIXNUM) {
                    throw RuntimeError("");
                }
            }
            return Expr(new GreaterEq(content[0],content[1]));
        }else if(str == ">") {
            if(n != 3) {
                throw RuntimeError("");
            }
            for(int i = 1;i < n;++i) {
                content.push_back(list_ -> stxs[i].parse(env));
                if(content.back()->e_type != E_FIXNUM) {
                    throw RuntimeError("");
                }
            }
            return Expr(new GreaterEq(content[0],content[1]));
        }else if(str == "not") {
            if(n != 2) {
                throw RuntimeError("");
            }
            content.push_back(list_ -> stxs[1].parse(env));
            return Expr(new Not(content[0]));
        }else if(str == "fixnum?") {
            if(n != 2) {
                throw RuntimeError("");
            }
            content.push_back(list_ -> stxs[1].parse(env));
            return Expr(new IsFixnum(content[0]));
        }else if(str == "boolean?") {
            if(n != 2) {
                throw RuntimeError("");
            }
            content.push_back(list_ -> stxs[1].parse(env));
            return Expr(new IsBoolean(content[0]));
        }else if(str == "null?") {
            if(n != 2) {
                throw RuntimeError("");
            }
            content.push_back(list_ -> stxs[1].parse(env));
            return Expr(new IsNull(content[0]));
        }else if(str == "pair?") {
            if(n != 2) {
                throw RuntimeError("");
            }
            content.push_back(list_ -> stxs[1].parse(env));
            return Expr(new IsPair(content[0]));
        }else if(str == "symbol?") {
            if(n != 2) {
                throw RuntimeError("");
            }
            content.push_back(list_ -> stxs[1].parse(env));
            return Expr(new IsSymbol(content[0]));
        }else if(str == "eq?") {
            if(n != 3) {
                throw RuntimeError("");
            }
            content.push_back(list_ -> stxs[1].parse(env));
            content.push_back(list_ -> stxs[2].parse(env));
            return Expr(new IsEq(content[0],content[1]));
        }else if(str == "lambda") {
            if(n < 3) {
                throw RuntimeError("");
            }
            std::vector<std::string> v_str;
            if(List* list_1 =dynamic_cast<List*>(list_->stxs[1].get())) {
                for(int i = 0;i < list_1->stxs.size();++i) {
                    if(Identifier* ide = dynamic_cast<Identifier*>(list_1->stxs[i].get())) {
                        v_str.push_back(ide->s);
                        continue;
                    }
                    throw RuntimeError("");
                }
            }else if(Identifier* ide =dynamic_cast<Identifier*>(list_->stxs[1].get())) {
                v_str.push_back(ide->s);
            }
            std::vector<Expr> es;
            for(int i = 2;i < list_->stxs.size();++i) {
                es.push_back(list_->stxs[i]->parse(env));
            }
            return Expr(new Lambda(v_str,Expr(new Begin(es))));
        }else if(str == "let") {
            if(n != 3) {
                throw RuntimeError("");
            }
            std::vector<std::pair<std::string, Expr>> bind;
            if(List* list_l = dynamic_cast<List*>(list_->stxs[1].get())) {
                int size = list_l->stxs.size();
                for(int i = 0;i < size; ++i) {
                    if(List* list_li = dynamic_cast<List*>(list_l->stxs[i].get())) {
                        int size_2 = list_li->stxs.size();
                        if(size_2 != 2) {
                            throw RuntimeError("");
                        }
                        if(Identifier* iden = dynamic_cast<Identifier*>(list_li->stxs[0].get())) {
                            bind.push_back({iden->s,list_li->stxs[1].parse(env)});
                        }
                    }
                }
            }
            Expr body = Expr(new Quote(list_->stxs[2]));
            return Expr(new Let(bind,body));
        }
    }else if (List* transform_Syntax = dynamic_cast<List*>(operator_Syntax)) {
        Expr exp = transform_Syntax->parse(env);
        std::vector<Expr> rand;
        for(int i = 1;i < n;++i) {
            rand.push_back(list_->stxs[i].parse(env));
        }
        return Expr(new Apply(exp,rand));
    }
    throw RuntimeError("");
}

#endif