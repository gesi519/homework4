#ifndef PARSER 
#define PARSER

// parser of myscheme 

/*
不知道这算是重构的第几版本了

第一个版本，跟着README做，发现到了运算符（原函数）的再定义就不行了

于是第二版诞生了，我把primitives里的解释全放到了evaluation里了，但是这样埋下了隐患，reserved_words的重载没有解决

由于长时间没有得不到正确答案，我选择瞎猫碰见死耗子，继续重构，我就把严格的把let和leterc的作用域绑定进行，然而因此我大幅度篡改那些不用改的文件，
如ExprBase 我把它从纯虚函数改成了普通的虚函数
又如我把没有用到的String类型给补全了，但是还是会有错
经过长时间的调试，我意识到这样做的话，lambda表达式无法实现不求值以及我也深刻的明白不可能这么大幅度的改那些文件，然后我就改了回来

最后这一版代码诞生了，是它前辈们的杂交成果。同时我发现StringV("")完全可以用NullV()和TerminateV()来代替。检查reserved_words的重载，把primitives当成普通函数和它的实参的组合。
最后经过不断利用数据和输出调试锁定了是作用域的问题，最后经过了四天什么除了大作业都没有干和之前几天对于第一版的理解，终于写出来了


已经交完了，突然想起来之前的一个疑问，如果lambda表达式里的参数列表里是 123.4，按照README里如果字符串可以被识别为一个数字，那么它会被优先识别为数字，例如 1、-1、+123、.123、+124.、1e-3
这个算字符串还是数字？
所以输入这些到lambda会不会报错？
我下载的scheme会报错，但是我的scheme会先将其识别为字符串，如果调用就会报错，单纯的lambda是不会错的
本来想要做这个转化的，lambda里多余的那步把把参数转化为Var类型，就是为了这，但是时间不够了，也怕麻烦，我就不做了
*/

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
                            env_new = extend(va->x,NullV(),env_new);
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
                                bind.push_back(mp(iden->s,list_li->stxs[1].parse(env)));
                                env_new = extend(iden->s,NullV(),env_new);
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
                                env_new = extend(iden->s,NullV(),env_new);
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
                                bind.push_back(mp(iden->s,list_li->stxs[1].get()->parse(env_new)));
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
    throw RuntimeError("");
}

#endif
