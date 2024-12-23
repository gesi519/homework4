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

Value Let::eval(Assoc &env) { // let expression
    Assoc new_env = env;
    int size = bind.size();
    for(int i = 0;i < size;++i) {

        std::string &str = bind[i].first;
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
        if(clos->e->e_type == E_VAR) {
            Var* var = dynamic_cast<Var*>(clos -> e.get());
            if(find(var->x,e_tmp).get()) {

            }else if(primitives.count(var -> x)) {
                if(rand.size() == 2) {
                    if(primitives[var -> x] == E_MUL) {
                        return Mult(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_MINUS) {
                        return Minus(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_PLUS) {
                        return Plus(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_LT) {
                        return Less(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_LE) {
                        return LessEq(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_EQ) {
                        return Equal(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_GE) {
                        return GreaterEq(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_GT) {
                        return Greater(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_EQQ) {
                        return IsEq(rand[0],rand[1]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_CONS) {
                        return Cons(rand[0],rand[1]).eval(e_tmp);
                    }
                }else if(rand.size() == 1) {
                    if(primitives[var -> x] == E_BOOLQ) {
                        return Expr(new IsBoolean(rand[0])) -> eval(e_tmp);
                    }else if(primitives[var -> x] == E_INTQ) {
                        return IsFixnum(rand[0]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_NULLQ) {
                        return IsNull(rand[0]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_PAIRQ) {
                        return IsPair(rand[0]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_PROCQ) {
                        return IsProcedure(rand[0]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_SYMBOLQ) {
                        return IsSymbol(rand[0]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_NOT) {
                        return Not(rand[0]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_CAR) {
                        return Expr(new Car(rand[0])) -> eval(e_tmp);
                    }else if(primitives[var -> x] == E_CDR) {
                        return Expr(new Cdr(rand[0])) -> eval(e_tmp);
                    }
                }else if(rand.size() == 0) {
                    if(primitives[var -> x] == E_EXIT) {
                        return Exit().eval(e_tmp);
                    }else if(primitives[var -> x] == E_VOID) {
                        return MakeVoid().eval(e_tmp);
                    }
                }
            }
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
        Expr &expr = bind[i].second;
        env = extend(str, Value(nullptr), env);
    }
    std::vector<Value> val_v;

    for(int i = 0;i < size;++i) {
        std::string &str = bind[i].first;
        Expr &expr = bind[i].second;
        Value val = expr->eval(env);
        val_v.push_back(val);
    }
    for(int i = 0;i < size;++i) {
        std::string &str = bind[i].first;
        modify(str, val_v[i], env);
    }
    Value body_value1 = body ->eval(env);
    env = new_env;
    return body_value1;
}

Value Var::eval(Assoc &e) { // evaluation of variable
    Value val = find(x,e);
    if(val.get()) {
        return val;
    }else if(primitives.count(x)) {
        std::vector<std::string> v;
        if(primitives1.count(x)) {
            v.push_back("rand");
        }else if(primitives2.count(x)) {
            v.push_back("rand1");
            v.push_back("rand2");
        }
        return ClosureV(v,Expr(new Var(x)),e);
    }else if(reserved_words.count(x)) {
        std::vector<std::string> v(1,x);
        return ClosureV(v,Expr(new Var(x)),e);
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
    return Value(es.back()->eval(e));
}

Value Quote::eval(Assoc &e) { // quote expression
    if(Identifier* ide = dynamic_cast<Identifier*>(this->s.get())) {
        return SymbolV(ide->s);
    }else if(List* list_ = dynamic_cast<List*>(this->s.get())) {
        if(list_->stxs.size() == 0) {
            return NullV();
        }
        Syntax syntax_tmp = list_->stxs[0];
        List rest_list = *list_;
        rest_list.stxs.erase(rest_list.stxs.begin());
        List* list_ptr = new List(rest_list);
        if(rest_list.stxs.size() == 2) {
            if(Identifier* ide = dynamic_cast<Identifier*>(rest_list.stxs[0].get())) {
                if(ide->s == ".") {
                    return PairV(Quote(syntax_tmp).eval(e),Quote(rest_list.stxs[1]).eval(e));
                }
            }
        }
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
        Pair* pa1 = pa;
        if(pa1->cdr->v_type == V_PAIR) {
            Pair* pa1 = dynamic_cast<Pair*>(pa->cdr.get());
            if(Symbol* sym = dynamic_cast<Symbol*>(pa1->car.get())) {
                if(sym->s == ".") {
                    return pa1->cdr;
                }
            }
        }
        return pa->cdr;
    }
    throw RuntimeError("");
}