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

Value Let::eval(Assoc &env) { // let expression
    return Value(nullptr);
}
Value Lambda::eval(Assoc &env) { // lambda expression
    return Value(nullptr);
}
Value Apply::eval(Assoc &e) { // for function calling
    return Value(nullptr);
}
Value Letrec::eval(Assoc &env) { // letrec expression
    return Value(nullptr);
}
Value Var::eval(Assoc &e) { // evaluation of variable
    return Value(nullptr);
}
Value Fixnum::eval(Assoc &e) { // evaluation of a fixnum
    return Value(new Integer(n));
}
Value If::eval(Assoc &e) { // if expression
    return Value(nullptr);
}
Value True::eval(Assoc &e) { // evaluation of #t
    return Value(nullptr);
}
Value False::eval(Assoc &e) { // evaluation of #f
    return Value(nullptr);
}
Value Begin::eval(Assoc &e) { // begin expression
    return Value(nullptr);
}
Value Quote::eval(Assoc &e) { // quote expression
    return Value(nullptr);
}
Value MakeVoid::eval(Assoc &e) { // (void)
    return Value(nullptr);
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
    return Value(nullptr);
}
Value Mult::evalRator(const Value &rand1, const Value &rand2) { // *
    Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
    Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());

    return Value(new Integer(tmp1-> n * tmp2 -> n));
}
Value Plus::evalRator(const Value &rand1, const Value &rand2) { // +
    Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
    Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());

    return Value(new Integer(tmp1-> n + tmp2 -> n));
}
Value Minus::evalRator(const Value &rand1, const Value &rand2) { // -
    Integer* tmp1 = dynamic_cast<Integer*>(rand1.get());
    Integer* tmp2 = dynamic_cast<Integer*>(rand2.get());

    return Value(new Integer(tmp1-> n - tmp2 -> n));
}
Value Less::evalRator(const Value &rand1, const Value &rand2) { // <
    return Value(nullptr);
}
Value LessEq::evalRator(const Value &rand1, const Value &rand2) { // <=
    return Value(nullptr);
}
Value Equal::evalRator(const Value &rand1, const Value &rand2) { // =
    return Value(nullptr);
}
Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) { // >=
    return Value(nullptr);
}
Value Greater::evalRator(const Value &rand1, const Value &rand2) { // >
    return Value(nullptr);
}
Value IsEq::evalRator(const Value &rand1, const Value &rand2) { // eq?
    return Value(nullptr);
}
Value Cons::evalRator(const Value &rand1, const Value &rand2) { // cons
    return Value(nullptr);
}
Value IsBoolean::evalRator(const Value &rand) { // boolean?
    return Value(nullptr);
}
Value IsFixnum::evalRator(const Value &rand) { // fixnum?
    return Value(nullptr);
}
Value IsSymbol::evalRator(const Value &rand) { // symbol?
    return Value(nullptr);
}
Value IsNull::evalRator(const Value &rand) { // null?
    return Value(nullptr);
}
Value IsPair::evalRator(const Value &rand) { // pair?
    return Value(nullptr);
}
Value IsProcedure::evalRator(const Value &rand) { // procedure?
    return Value(nullptr);
}
Value Not::evalRator(const Value &rand) { // not
    return Value(nullptr);
}
Value Car::evalRator(const Value &rand) { // car
    return Value(nullptr);
}
Value Cdr::evalRator(const Value &rand) { // cdr
    return Value(nullptr);
}