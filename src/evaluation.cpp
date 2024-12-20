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
    Quote* qu = dynamic_cast<Quote*>(body.get());
    return qu->s->parse(new_env)->eval(new_env);
}

Value Lambda::eval(Assoc &env) { // lambda expression
    return ClosureV(x,e,env);
}

Value Apply::eval(Assoc &e) { // for function calling
    Value val = rator->eval(e);
    if(Closure* clos = dynamic_cast<Closure*>(val.get())) {
        int size = clos->parameters.size();
        std::cout<<size<<std::endl;
        std::cout<<rand.size()<<std::endl;
        if(size != rand.size()) {
            throw RuntimeError("");
        }
        Assoc e_tmp = e;
        for(int i = 0;i < size;++i){
            e_tmp = extend(clos->parameters[i],rand[i]->eval(e_tmp),e_tmp);
        }
        if(Var* var = dynamic_cast<Var*>(clos -> e.get())) {
            if(primitives.count(var -> x)) {
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
                        return IsBoolean(rand[0]).eval(e_tmp);
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
                        return Car(rand[0]).eval(e_tmp);
                    }else if(primitives[var -> x] == E_CDR) {
                        return Cdr(rand[0]).eval(e_tmp);
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
        return clos->e->eval(e_tmp);
    }
    throw RuntimeError("");
}

Value Letrec::eval(Assoc &env) { // letrec expression
    return Value(nullptr);
}

Value Var::eval(Assoc &e) { // evaluation of variable
    if(x[0] == '.'||(x[0] == '@'||x[0] - '0' >= 0 &&x[0] - '0' <=9)) {
        throw RuntimeError("");
    }
    if(find(x,e).get()) {
        return find(x,e);
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
    if(Boolean* boo = dynamic_cast<Boolean*>(val.get())) {
        if(boo->b) {
            return conseq->eval(e);
        }else {
            return alter->eval(e);
        }
    }
    throw RuntimeError("");
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
        return SymbolV("Unspecified return value");
    }
    for(int i = 1;i < es.size();++i) {
        es[i]->eval(e);
    }
    return Value(es.back()->eval(e));
}

Value Quote::eval(Assoc &e) { // quote expression
    if(Identifier* ide = dynamic_cast<Identifier*>(this->s.get())) {
        return SymbolV(ide->s);
    }else if(Number* num = dynamic_cast<Number*>(this->s.get())) {
        return IntegerV(num -> n);
    }else if(TrueSyntax* boo = dynamic_cast<TrueSyntax*>(this->s.get())) {
        return BooleanV(true);
    }else if(FalseSyntax* boo = dynamic_cast<FalseSyntax*>(this->s.get())) {
        return BooleanV(false);
    }else if(List* list_ = dynamic_cast<List*>(this->s.get())) {
        if(list_->stxs.size() == 0) {
            return NullV();
        }
        Syntax syntax_tmp = list_->stxs[0];
        list_->stxs.erase(list_->stxs.begin());
        if(list_->stxs.size() >= 1) {
            List list_1 = *list_;
            List* list_ptr = new List(list_1);
            return PairV(Quote(syntax_tmp).eval(e),Quote(list_ptr).eval(e));
        }else if(list_->stxs.size() == 0) {
            return PairV(Quote(syntax_tmp).eval(e),NullV());
        }
    } 
    throw RuntimeError("");
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
    if(Integer* tmp1 = dynamic_cast<Integer*>(rand1.get())) {
        if(Integer* tmp2 = dynamic_cast<Integer*>(rand2.get())) {
            return Value(new Integer(tmp1-> n * tmp2 -> n));
        }
    }
    throw RuntimeError("");
}

Value Plus::evalRator(const Value &rand1, const Value &rand2) { // +
    if(Integer* tmp1 = dynamic_cast<Integer*>(rand1.get())) {
        if(Integer* tmp2 = dynamic_cast<Integer*>(rand2.get())) {
            return Value(new Integer(tmp1-> n + tmp2 -> n));
        }
    }
    throw RuntimeError("");
}

Value Minus::evalRator(const Value &rand1, const Value &rand2) { // -
    if(Integer* tmp1 = dynamic_cast<Integer*>(rand1.get())) {
        if(Integer* tmp2 = dynamic_cast<Integer*>(rand2.get())) {
            return Value(new Integer(tmp1-> n - tmp2 -> n));
        }
    }
    throw RuntimeError("");
}

Value Less::evalRator(const Value &rand1, const Value &rand2) { // <
    Integer* num1 = dynamic_cast<Integer*>(rand1.get());
    Integer* num2 = dynamic_cast<Integer*>(rand2.get());
    if(num1->n < num2->n) {
        return Value(BooleanV(true));
    }
    return Value(BooleanV(false));
}

Value LessEq::evalRator(const Value &rand1, const Value &rand2) { // <=
    Integer* num1 = dynamic_cast<Integer*>(rand1.get());
    Integer* num2 = dynamic_cast<Integer*>(rand2.get());
    if(num1->n <= num2->n) {
        return Value(BooleanV(true));
    }
    return Value(BooleanV(false));
}

Value Equal::evalRator(const Value &rand1, const Value &rand2) { // =
    Integer* num1 = dynamic_cast<Integer*>(rand1.get());
    Integer* num2 = dynamic_cast<Integer*>(rand2.get());
    if(num1->n == num2->n) {
        return Value(BooleanV(true));
    }
    return Value(BooleanV(false));
}

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) { // >=
    Integer* num1 = dynamic_cast<Integer*>(rand1.get());
    Integer* num2 = dynamic_cast<Integer*>(rand2.get());
    if(num1->n >= num2->n) {
        return Value(BooleanV(true));
    }
    return Value(BooleanV(false));
}

Value Greater::evalRator(const Value &rand1, const Value &rand2) { // >
    Integer* num1 = dynamic_cast<Integer*>(rand1.get());
    Integer* num2 = dynamic_cast<Integer*>(rand2.get());
    if(num1->n > num2->n) {
        return Value(BooleanV(true));
    }
    return Value(BooleanV(false));
}

Value IsEq::evalRator(const Value &rand1, const Value &rand2) { // eq?
    if(rand1->v_type != rand2->v_type) {
        return BooleanV(false);
    }
    if(Integer* in1 = dynamic_cast<Integer*>(rand1.get())) {
        Integer* in2 = dynamic_cast<Integer*>(rand2.get());
        if(in1->n == in2->n) {
            return BooleanV(true);
        }
    }
    if(Null* nu = dynamic_cast<Null*>(rand1.get())) {
        return BooleanV(true);
    }
    if(Void* vo = dynamic_cast<Void*>(rand1.get())) {
        return BooleanV(true);
    }
    if(Symbol* sym1 = dynamic_cast<Symbol*>(rand1.get())) {
        Symbol* sym2 = dynamic_cast<Symbol*>(rand2.get());
        if(sym1->s == sym2->s) {
            return BooleanV(true);
        }
    }
    if(Pair* pai1 = dynamic_cast<Pair*>(rand1.get())) {
        Pair* pai2 = dynamic_cast<Pair*>(rand2.get());
        if(&pai1 == &pai2) {
            return BooleanV(true);
        }
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
    if(rand->v_type == V_INT) {
        return BooleanV(false);
    }else if(Boolean* boo = dynamic_cast<Boolean*>(rand.get())) {
        return BooleanV(!boo->b);
    }
    throw RuntimeError("");
}

Value Car::evalRator(const Value &rand) { // car
    if(Pair* pa = dynamic_cast<Pair*>(rand.get())) {
        return pa->car;
    }
    throw RuntimeError("");
}

Value Cdr::evalRator(const Value &rand) { // cdr
    if(Pair* pa = dynamic_cast<Pair*>(rand.get())) {
        Pair* pa1 = pa; 
        while(Pair* pa1 = dynamic_cast<Pair*>(pa->cdr.get())) {
            pa = pa1;
        }
        return pa->car;
    }
    throw RuntimeError("");
}