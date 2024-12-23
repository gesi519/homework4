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

Value ExprBase :: eval(Assoc &e) { throw RuntimeError("");}
Expr Syntax :: parse(Assoc &env) {
    return ptr -> parse(env);//自动调用它指向的Syntaxbase
}

Expr Number :: parse(Assoc &env) {
    return Expr(new Fixnum(n));
}

Expr Identifier :: parse(Assoc &env) {
    if (find(s, env).get()){
        return Expr(new Var(s));
    }else if (primitives.count(s)) {
        return Expr(new ExprBase(primitives[s]));
    }else if(reserved_words.count(s)) {
        return Expr(new ExprBase(reserved_words[s]));
    }
    throw RuntimeError("RuntimeError Identifier");
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
    if(n==0) {
        return Expr(new Quote(Syntax(new List)));
    }
    if(List* first = dynamic_cast<List*>(stxs[0].get())) {
        std::vector<Expr> ex_v;
        for(int i = 1;i < stxs.size();++i) {
            ex_v.push_back(stxs[i]->parse(env));
        }
        return Expr(new Apply(first ->parse(env),ex_v));
    }
    Expr first = stxs[0]->parse(env);
    //primitives 原生函数
    if(first -> e_type == E_MUL) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new Mult(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_MINUS) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new Minus(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_PLUS) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new Plus(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_LT) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new Less(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_LE) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new LessEq(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_EQ) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new Equal(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_GE) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new GreaterEq(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_GT) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new Greater(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_EQQ) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new IsEq(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_CONS) {
        if(n != 3) {
            throw RuntimeError("");
        }
        return Expr(new Cons(stxs[1]->parse(env),stxs[2]->parse(env)));
    }else if(first -> e_type == E_BOOLQ) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new IsBoolean(stxs[1]->parse(env)));
    }else if(first -> e_type == E_INTQ) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new IsFixnum(stxs[1]->parse(env)));
    }else if(first -> e_type == E_NULLQ) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new IsNull(stxs[1]->parse(env)));
    }else if(first -> e_type == E_PAIRQ) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new IsPair(stxs[1]->parse(env)));
    }else if(first -> e_type == E_PROCQ) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new IsProcedure(stxs[1]->parse(env)));
    }else if(first -> e_type == E_SYMBOLQ) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new IsSymbol(stxs[1]->parse(env)));
    }else if(first -> e_type == E_NOT) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new Not(stxs[1]->parse(env)));
    }else if(first -> e_type == E_CAR) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new Car(stxs[1]->parse(env)));
    }else if(first -> e_type == E_CDR) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new Cdr(stxs[1]->parse(env)));
    }else if(first -> e_type == E_EXIT) {
        if(n != 1) {
            throw RuntimeError("");
        }
        return Expr(new Exit());
    }else if(first -> e_type == E_VOID) {
        if(n != 1) {
            throw RuntimeError("");
        }
        return Expr(new MakeVoid());
    }//reserved_words 类型
    else if(first->e_type == E_QUOTE) {
        if(n != 2) {
            throw RuntimeError("");
        }
        return Expr(new Quote(stxs[1]));
    }else if(first->e_type == E_BEGIN) {
        std::vector<Expr> content;
        for(int i = 1;i < n;++i) {
            content.push_back(stxs[i].parse(env));
        }
        return Expr(new Begin(content));
    }else if(first->e_type == E_IF) {
        if(n != 4) {
            throw RuntimeError("");
        }
        std::vector<Expr> content;
        for(int i = 1;i < n;++i) {
            content.push_back(stxs[i].parse(env));
        }
        return Expr(new If(content[0],content[1],content[2]));
    }else if(first->e_type == E_LAMBDA) {
        if(n != 3) {
            throw RuntimeError("");
        }
        std::vector<std::string> v_str;
        Assoc env_new = env;
        if(List* list_ =dynamic_cast<List*>(stxs[1].get())) {
            for(int i = 0;i < list_->stxs.size();++i) {
                if(Identifier* ide = dynamic_cast<Identifier*>(list_->stxs[i].get())) {
                    v_str.push_back(ide->s);
                    env_new = extend(ide -> s,StringV(""),env_new);
                    continue;
                }
                throw RuntimeError("");
            }
        }else if(Identifier* ide =dynamic_cast<Identifier*>(stxs[1].get())) {
            v_str.push_back(ide->s);
            env_new = extend(ide -> s,StringV(""),env_new);
        }
        Expr ex = stxs[2]->parse(env_new);
        return Expr(new Lambda(v_str,ex));
    }else if(first->e_type == E_LET) {
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
                        bind.push_back({iden->s,list_li->stxs[1].parse(env_new)});
                        
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
    }else if(first->e_type == E_LETREC) {
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
            }else {
                throw RuntimeError("");
            }
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
                        }
                    }
                }
            }
            Expr body = stxs[2]->parse(env_new);
            return Expr(new Letrec(bind,body));
    }else if(first->e_type == E_VAR) {
        std::vector<Expr> ex_v;
        for(int i = 1;i < stxs.size();++i) {
            ex_v.push_back(stxs[i]->parse(env));
        }
        return Expr(new Apply(first,ex_v));
    }

    throw RuntimeError("RuntimeError List");
}

#endif