#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;
extern std :: map<std :: string, ExprType> primitives0;
extern std :: map<std :: string, ExprType> primitives1;
extern std :: map<std :: string, ExprType> primitives2;


void checkname(std::string& str) {
    if(str.empty()) {
        throw RuntimeError("");
    }
    if(std::isdigit(str[0]) || str[0] == '.' || str[0] == '@')
        throw RuntimeError("");
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == '#') {
            throw RuntimeError("");
        }
    }
    return;
}

Value Let::eval(Assoc &env) { // let expression
    Assoc new_env = env;
    int size = bind.size();
    for(int i = 0;i < size;++i) {
        std::string &str = bind[i].first;
        checkname(str);
        Expr &expr = bind[i].second;
        Value val = expr->eval(env);
        new_env = extend(str, val, new_env);
    }
    return body ->eval(new_env);
}

Value Lambda::eval(Assoc &env) { // lambda expression
    return ClosureV(x,e,env);
}

Value Apply::eval(Assoc &e) { // for function calling
    Value val = rator->eval(e);
    if(val->v_type == V_PROC) {
        Closure* clos = dynamic_cast<Closure*>(val.get());
        int size = clos->parameters.size();
        if(size != rand.size()) {
            throw RuntimeError("");
        }
        Assoc e_tmp = clos->env;
        for(int i = 0;i < size;++i) {
            e_tmp = extend(clos->parameters[i],rand[i]->eval(e),e_tmp);
        }
        return (clos->e)->eval(e_tmp);
    }
    throw RuntimeError("");
}

Value Letrec::eval(Assoc &env) { // letrec expression
    Assoc new_env = env;
    int size = bind.size();
    for(int i = 0;i < size;++i) {
        std::string &str = bind[i].first;
        checkname(str);
        Expr &expr = bind[i].second;
        env = extend(str, Value(nullptr), new_env);
    }
    std::vector<Value> val_v;
    for(int i = 0;i < size;++i) {
        std::string &str = bind[i].first;
        Expr &expr = bind[i].second;
        Value val = expr->eval(new_env);
        val_v.push_back(val);
    }
    Assoc env1 = new_env;
    for(int i = 0;i < size;++i) {
        std::string &str = bind[i].first;
        modify(str, val_v[i], env1);
    }
    Value body_value = body ->eval(env1);
    return body_value;
}

Value Var::eval(Assoc &e) { // evaluation of variable
    checkname(x);
    Value val = find(x,e);
    if(val.get()) {
        if(val->v_type != V_STRING){
            return val;
        }else {
            throw RuntimeError("");
        }
    }else if(primitives.count(x)) {
        std::vector<std::string> v;
        if(primitives[x] == E_PLUS) {
            v.push_back("plus1");
            v.push_back("plus2");
            Expr expr = Expr(new Plus(new Var("plus1"),new Var("plus2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_MINUS) {
            v.push_back("minus1");
            v.push_back("minus2");
            Expr expr = Expr(new Minus(new Var("minus1"),new Var("minus2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_MUL) {
            v.push_back("mul1");
            v.push_back("mul2");
            Expr expr = Expr(new Mult(new Var("mul1"),new Var("mul2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_LT) {
            v.push_back("lt1");
            v.push_back("lt2");
            Expr expr = Expr(new Less(new Var("lt1"),new Var("lt2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_LE) {
            v.push_back("le1");
            v.push_back("le2");
            Expr expr = Expr(new LessEq(new Var("le1"),new Var("le2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_EQ) {
            v.push_back("eq1");
            v.push_back("eq2");
            Expr expr = Expr(new Equal(new Var("eq1"),new Var("eq2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_GE) {
            v.push_back("ge1");
            v.push_back("ge2");
            Expr expr = Expr(new GreaterEq(new Var("ge1"),new Var("ge2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_GT) {
            v.push_back("gt1");
            v.push_back("gt2");
            Expr expr = Expr(new Greater(new Var("gt1"),new Var("gt2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_EQQ) {
            v.push_back("eqq1");
            v.push_back("eqq2");
            Expr expr = Expr(new IsEq(new Var("eqq1"),new Var("eqq2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_BOOLQ) {
            v.push_back("boolq");
            Expr expr = Expr(new IsBoolean(new Var("boolq")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_INTQ) {
            v.push_back("intq");
            Expr expr = Expr(new IsFixnum(new Var("intq")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_NULLQ) {
            v.push_back("nullq");
            Expr expr = Expr(new IsNull(new Var("nullq")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_PAIRQ) {
            v.push_back("pairq");
            Expr expr = Expr(new IsPair(new Var("pairq")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_PROCQ) {
            v.push_back("procq");
            Expr expr = Expr(new IsProcedure(new Var("procq")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_SYMBOLQ) {
            v.push_back("symbolq");
            Expr expr = Expr(new IsSymbol(new Var("symbolq")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_CONS) {
            v.push_back("cons1");
            v.push_back("cons2");
            Expr expr = Expr(new Cons(new Var("cons1"),new Var("cons2")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_NOT) {
            v.push_back("not");
            Expr expr = Expr(new Not(new Var("symbolq")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_CAR) {
            v.push_back("car");
            Expr expr = Expr(new Car(new Var("car")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_CDR) {
            v.push_back("cdr");
            Expr expr = Expr(new Cdr(new Var("cdr")));
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_VOID) {
            Expr expr = Expr(new MakeVoid());
            return ClosureV(v, expr, e);
        }else if(primitives[x] == E_EXIT) {
            Expr expr = Expr(new Exit());
            return ClosureV(v, expr, e);
        }
    }
    throw RuntimeError("");
}

Value Fixnum::eval(Assoc &e) { // evaluation of a fixnum
    return Value(new Integer(n));
}

Value If::eval(Assoc &e) { // if expression
    Value val = cond.get()->eval(e);
    if(val.get()->v_type == V_BOOL) {
        Boolean* boo = dynamic_cast<Boolean*>(val.get());
        if(boo->b) {
            return conseq->eval(e);
        }else {
            return alter->eval(e);
        }
    }
    return conseq->eval(e);
}

// -> Boolean
Value True::eval(Assoc &e) { // evaluation of #t
    return BooleanV(true);
}
Value False::eval(Assoc &e) { // evaluation of #f
    return BooleanV(false);
}

Value Begin::eval(Assoc &e) { // begin expression
    if(es.empty()) {
        return NullV();
    }
    for(int i = 0;i < es.size() - 1;++i) {
        es[i]->eval(e);
    }
    return es.back()->eval(e);
}

Value Quote::eval(Assoc &e) { // quote expression
    if(Identifier* ide = dynamic_cast<Identifier*>(this->s.get())) {
        return SymbolV(ide->s);
    }else if(List* list_ = dynamic_cast<List*>(this->s.get())) {
        if(list_->stxs.size() == 0) {
            return NullV();
        }else if(list_->stxs.size() == 1) {
            return PairV(Quote(list_->stxs[0]).eval(e),NullV());
        }else {
            int pos=-1;
            int count = 0;
            for(int i = 0;i < list_->stxs.size();++i) {
                if(Identifier* id = dynamic_cast<Identifier*>(list_->stxs[i].get())) {
                    if(id->s == ".") {
                        count++;
                        pos = i;
                        if(count > 1) {
                            throw RuntimeError("");
                        }
                    }
                }
            }
            if(count == 1 && ((pos != list_->stxs.size() - 2) || list_ ->stxs.size() < 3)) {
                throw RuntimeError("");
            }

            if(list_->stxs.size() == 3) {
                if(Identifier* id = dynamic_cast<Identifier*>(list_->stxs[1].get())) {
                    if(id->s == ".") {
                        return PairV(Quote(list_->stxs[0]).eval(e),Quote(list_->stxs[2]).eval(e));
                    }
                }
            }
        }
        Syntax syntax_tmp = list_->stxs[0];
        List rest_list = *list_;
        rest_list.stxs.erase(rest_list.stxs.begin());
        List* list_ptr = new List(rest_list);
        return PairV(Quote(syntax_tmp).eval(e),Quote(list_ptr).eval(e));
    }else {
        return s->parse(e)->eval(e);
    }
}

Value MakeVoid::eval(Assoc &e) { // (void)
    return VoidV();
}

Value Exit::eval(Assoc &e) { // (exit)
    return Value(new Terminate());
}

Value Binary::eval(Assoc &e) { // evaluation of two-operators primitive
    Value v1 = rand1 -> eval(e);
    Value v2 = rand2 -> eval(e);
    return evalRator(v1,v2);
}

Value Unary::eval(Assoc &e) { // evaluation of single-operator primitive
    Value v = rand -> eval(e);
    return evalRator(v);
}

Value Mult::evalRator(const Value &rand1, const Value &rand2) { // *
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return Value(new Integer(tmp1-> n * tmp2 -> n));
    }
    throw RuntimeError("");
}

Value Plus::evalRator(const Value &rand1, const Value &rand2) { // +
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return Value(new Integer(tmp1-> n + tmp2 -> n));
    }
    throw RuntimeError("");
}

Value Minus::evalRator(const Value &rand1, const Value &rand2) { // -
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return Value(new Integer(tmp1-> n - tmp2 -> n));
    }
    throw RuntimeError("");
}

Value Less::evalRator(const Value &rand1, const Value &rand2) { // <
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return BooleanV(tmp1->n < tmp2->n);
    }
    throw RuntimeError("");
}

Value LessEq::evalRator(const Value &rand1, const Value &rand2) { // <=
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return BooleanV(tmp1->n <= tmp2->n);
    }
    throw RuntimeError("");
}

Value Equal::evalRator(const Value &rand1, const Value &rand2) { // =
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return BooleanV(tmp1->n == tmp2->n);
    }
    throw RuntimeError("");
}

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) { // >=
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return BooleanV(tmp1->n >= tmp2->n);
    }
    throw RuntimeError("");
}

Value Greater::evalRator(const Value &rand1, const Value &rand2) { // >
    if(rand1.get()->v_type == V_INT && rand2.get()->v_type == V_INT) {
        Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
        Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());
        return BooleanV(tmp1->n > tmp2->n);
    }
    throw RuntimeError("");
}

Value IsEq::evalRator(const Value &rand1, const Value &rand2) { // eq?
    if(rand1->v_type != rand2->v_type) {
        return BooleanV(false);
    }
    if(rand1->v_type == V_INT) {
        Integer* in1 = dynamic_cast<Integer*>(rand1.get());
        Integer* in2 = dynamic_cast<Integer*>(rand2.get());
        if(in1->n == in2->n) {
            return BooleanV(true);
        }
    }
    if(rand1->v_type == V_NULL) {
        return BooleanV(true);
    }
    if(rand1->v_type == V_VOID) {
        return BooleanV(true);
    }
    if(rand1->v_type == V_SYM) {
        Symbol* sym1 = dynamic_cast<Symbol*>(rand1.get());
        Symbol* sym2 = dynamic_cast<Symbol*>(rand2.get());
        if(sym1->s == sym2->s) {
            return BooleanV(true);
        }
    }
    if(rand1->v_type == V_PAIR) {
        Pair* pai1 = dynamic_cast<Pair*>(rand1.get());
        Pair* pai2 = dynamic_cast<Pair*>(rand2.get());
        if(pai1 == pai2) {
            return BooleanV(true);
        }
    }
    if(rand1->v_type == V_BOOL) {
        Boolean* boo1 = dynamic_cast<Boolean*>(rand1.get());
        Boolean* boo2 = dynamic_cast<Boolean*>(rand2.get());
        return BooleanV(boo1->b == boo2->b);
    }
    if(rand1.get() == rand2.get()) {
        return BooleanV(true);
    }
    return BooleanV(false);
}

Value Cons::evalRator(const Value &rand1, const Value &rand2) { // cons
    return PairV(rand1,rand2);
}

Value IsBoolean::evalRator(const Value &rand) { // boolean?
    if(rand->v_type == V_BOOL) {
        return BooleanV(true);
    }
    return BooleanV(false);
}

Value IsFixnum::evalRator(const Value &rand) { // fixnum?
    if(rand->v_type == V_INT) {
        return BooleanV(true);
    }
    return BooleanV(false);
}

Value IsSymbol::evalRator(const Value &rand) { // symbol?
    if(rand->v_type == V_SYM) {
        return BooleanV(true);
    }
    return BooleanV(false);
}

Value IsNull::evalRator(const Value &rand) { // null?
    if(rand->v_type == V_NULL) {
        return BooleanV(true);
    }
    return BooleanV(false);
}

Value IsPair::evalRator(const Value &rand) { // pair?
    if(rand->v_type == V_PAIR) {
        return BooleanV(true);
    }
    return BooleanV(false);
}

Value IsProcedure::evalRator(const Value &rand) { // procedure?
    if(rand->v_type == V_PROC) {
        return BooleanV(true);
    }
    return BooleanV(false);
}

Value Not::evalRator(const Value &rand) { // not
    if(rand->v_type == V_BOOL) {
        Boolean* boo = dynamic_cast<Boolean*>(rand.get());
        return BooleanV(!boo->b);
    }
    return BooleanV(false);
}

Value Car::evalRator(const Value &rand) { // car
    if(rand->v_type == V_PAIR) {
        Pair* pa = dynamic_cast<Pair*>(rand.get());
        return pa->car;
    }
    throw RuntimeError("");
}

Value Cdr::evalRator(const Value &rand) { // cdr
    if(rand->v_type == V_PAIR) {
        Pair* pa = dynamic_cast<Pair*>(rand.get());
        return pa->cdr;
    }
    throw RuntimeError("");
}