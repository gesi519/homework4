#include "Def.hpp"

std :: map<std :: string, ExprType> primitives;
std :: map<std :: string, ExprType> reserved_words;
std :: map<std :: string, ExprType> primitives0;
std :: map<std :: string, ExprType> primitives1;
std :: map<std :: string, ExprType> primitives2;
void initPrimitives()
{
    // primitives stores all procedures in library, mapping them to ExprTypes
    primitives["*"] = E_MUL;
    primitives["-"] = E_MINUS;
    primitives["+"] = E_PLUS;
    primitives["<"] = E_LT;
    primitives["<="] = E_LE;
    primitives["="] = E_EQ;
    primitives[">="] = E_GE;
    primitives[">"] = E_GT;
    primitives["void"] = E_VOID;
    primitives["eq?"] = E_EQQ;
    primitives["boolean?"] = E_BOOLQ;
    primitives["fixnum?"] = E_INTQ;
    primitives["null?"] = E_NULLQ;
    primitives["pair?"] = E_PAIRQ;
    primitives["procedure?"] = E_PROCQ;
    primitives["symbol?"] = E_SYMBOLQ;
    primitives["cons"] = E_CONS;
    primitives["not"] = E_NOT;
    primitives["car"] = E_CAR;
    primitives["cdr"] = E_CDR;
    primitives["exit"] = E_EXIT;

    primitives0["void"] = E_VOID;
    primitives0["exit"] = E_EXIT;

    primitives2["*"] = E_MUL;
    primitives2["-"] = E_MINUS;
    primitives2["+"] = E_PLUS;
    primitives2["<"] = E_LT;
    primitives2["<="] = E_LE;
    primitives2["="] = E_EQ;
    primitives2[">="] = E_GE;
    primitives2[">"] = E_GT;
    primitives2["eq?"] = E_EQQ;
    primitives2["cons"] = E_CONS;

    primitives1["boolean?"] = E_BOOLQ;
    primitives1["fixnum?"] = E_INTQ;
    primitives1["null?"] = E_NULLQ;
    primitives1["pair?"] = E_PAIRQ;
    primitives1["procedure?"] = E_PROCQ;
    primitives1["symbol?"] = E_SYMBOLQ;
    primitives1["not"] = E_NOT;
    primitives1["car"] = E_CAR;
    primitives1["cdr"] = E_CDR;
}

void initReservedWords()
{
    // reserved_words stores all reserved words, mapping them to bools
    reserved_words["let"] = E_LET;
    reserved_words["lambda"] = E_LAMBDA;
    reserved_words["letrec"] = E_LETREC;
    reserved_words["if"] = E_IF;
    reserved_words["begin"] = E_BEGIN;
    reserved_words["quote"] = E_QUOTE;
}
