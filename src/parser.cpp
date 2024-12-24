#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include "value.hpp"
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

Expr Identifier :: parse(Assoc &env) {
    return Expr(new Var(s));
}

Expr TrueSyntax :: parse(Assoc &env) {
    return Expr(new True());
}

Expr FalseSyntax :: parse(Assoc &env) {
    return Expr(new False());
}

//主
Expr List :: parse(Assoc &env) {
    int n = stxs.size();
    if(n == 0) {
        throw RuntimeError("");
    }

    SyntaxBase* first = stxs[0].get();

    if(Identifier* ide = dynamic_cast<Identifier*>(first)) {
        std::string str = ide->s;
        Value val = find(str,env);
        if(val.get() != nullptr) { //变量已经被绑定了
            std::vector<Expr> ex;
            for(int i = 1;i < n;++i) {
                ex.push_back(stxs[i]->parse(env));
            }
            return Expr(new Apply(first->parse(env),ex));
        }
        if(reserved_words.count(str)) {
            if(reserved_words[str] == E_QUOTE) {
                if(n != 2) {
                    throw RuntimeError("");
                }
                return Expr(new Quote(stxs[1]));
            }else if(reserved_words[str] == E_BEGIN) {
                std::vector<Expr> content;
                for(int i = 1;i < n;++i) {
                    content.push_back(stxs[i].get()->parse(env));
                }
                return Expr(new Begin(content));
            }else if(reserved_words[str] == E_IF) {
                if(n != 4) {
                    throw RuntimeError("");
                }
                std::vector<Expr> content;
                for(int i = 1;i < n;++i) {
                    content.push_back(stxs[i].get()->parse(env));
                }
                return Expr(new If(content[0],content[1],content[2]));
            }else if(reserved_words[str] == E_LAMBDA) {
                if(n != 3) {
                    throw RuntimeError("");
                }
                std::vector<std::string> v_str;
                Assoc env_new = env;
                if(List* list_ =dynamic_cast<List*>(stxs[1].get())) {
                    for(int i = 0;i < list_->stxs.size();++i) {
                        Expr exp = list_->stxs[i].get()->parse(env);
                        if(Var* va = dynamic_cast<Var*>(exp.get())) {
                            v_str.push_back(va->x);
                            env_new = extend(va->x,StringV(""),env_new);
                            continue;
                        }
                        throw RuntimeError("");
                    }
                }else {
                    throw RuntimeError("");
                }
                Expr ex = stxs[2].get()->parse(env_new);
                return Expr(new Lambda(v_str,ex));
            }else if(reserved_words[str] == E_LET) {
                if(n != 3) {
                    throw RuntimeError("");
                }
                Assoc env_new = env;
                std::vector<std::pair<std::string, Expr>> bind;
                if(List* list_l = dynamic_cast<List*>(stxs[1].get())) {
                    int size = list_l->stxs.size();
                    for(int i = 0;i < size; ++i) {
                        if(List* list_li = dynamic_cast<List*>(list_l->stxs[i].get())) {
                            int size_2 = list_li->stxs.size();
                            if(size_2 != 2) {
                                throw RuntimeError("");
                            }
                            if(Identifier* iden = dynamic_cast<Identifier*>(list_li->stxs[0].get())) {
                                bind.push_back({iden->s,list_li->stxs[1].parse(env_new)});
                                env_new = extend(iden->s,StringV(""),env_new);
                            }else {
                                throw RuntimeError("");
                            }
                        }else {
                            throw RuntimeError("");
                        }
                    }
                }else {
                    throw RuntimeError("");
                }
                Expr body = stxs[2]->parse(env_new);
                return Expr(new Let(bind,body));
            }else if(reserved_words[str] == E_LETREC) {
                if(n != 3) {
                    throw RuntimeError("");
                }
                Assoc env_new = env;
                std::vector<std::pair<std::string, Expr>> bind;
                if(List* list_l = dynamic_cast<List*>(stxs[1].get())) {
                    int size = list_l->stxs.size();
                    for(int i = 0;i < size; ++i) {
                        if(List* list_li = dynamic_cast<List*>(list_l->stxs[i].get())) {
                            int size_2 = list_li->stxs.size();
                            if(size_2 != 2) {
                                throw RuntimeError("");
                            }
                            if(Identifier* iden = dynamic_cast<Identifier*>(list_li->stxs[0].get())) {
                                env_new = extend(iden->s,StringV(""),env_new);
                            }else {
                                throw RuntimeError("");
                            }
                        }else {
                            throw RuntimeError("");
                        }
                    }
                    for(int i = 0;i < size; ++i) {
                        if(List* list_li = dynamic_cast<List*>(list_l->stxs[i].get())) {
                            if(Identifier* iden = dynamic_cast<Identifier*>(list_li->stxs[0].get())) {
                                bind.push_back({iden->s,list_li->stxs[1].get()->parse(env_new)});
                            }
                        }
                    }
                }else {
                    throw RuntimeError("");
                }
                
                Expr body = stxs[2].get()->parse(env_new);
                return Expr(new Letrec(bind,body));
                }
            }

        vector<Expr> ex;  
        for (int i = 1; i < n; ++i) {
            ex.push_back(stxs[i]->parse(env));
        }
        return new Apply(first->parse(env),ex);
    }else if(List* lis = dynamic_cast<List*>(first)) {
        std::vector<Expr> ex_v;
        for(int i = 1;i < n;++i) {
            ex_v.push_back(stxs[i].get()->parse(env));
        }
        return Expr(new Apply(lis ->parse(env),ex_v));
    }
    std::vector<Expr> ex_v;
    for(int i = 1;i < n;++i) {
        ex_v.push_back(stxs[i].get()->parse(env));
    }
    return Expr(new Apply(first ->parse(env),ex_v));
}

#endif