//
//  Val.cpp
//  msdscript
//
//  Created by Nick Beckley on 2/23/21.
//

#include "Val.h"
#include "Expr.h"
#include "catch.h"
#include "Step.h"
#include "Cont.h"

std::string Val::to_string(){
    std::ostream stream(nullptr);
    std::stringbuf str;
    stream.rdbuf(&str);
    print(stream);
    return str.str();
}

NumVal::NumVal(int num){
    this->val = num;
}

bool NumVal::equals(PTR(Val) other){
    PTR(NumVal) num = CAST(NumVal)(other);
    if(num == NULL)
        return false;
    else
        return this->val == num->val;
}

PTR(Val) NumVal::add_to(PTR(Val) rhs){
    PTR(NumVal) other_num = CAST(NumVal)(rhs);
    if(other_num == NULL)
        throw std::runtime_error("add of non-number");
    return NEW(NumVal)(this->val + other_num->val);
}

PTR(Val) NumVal::mult_to(PTR(Val) rhs){
    PTR(NumVal) other_num = CAST(NumVal)(rhs);
    if(other_num == NULL)
        throw std::runtime_error("mult of non-number");
    return NEW(NumVal)(this->val * other_num->val);
}

bool NumVal::is_true(){
    throw std::runtime_error("Test expression is not a boolean");
}

PTR(Val) NumVal::call(PTR(Val) actual_arg){
    throw std::runtime_error("calling not allowed on numval");
}

void NumVal::call_step(PTR(Val) actual_arg, PTR(Cont) rest) {
    throw std::runtime_error("attempted to use call_step on a NumVal");
}

void NumVal::print(std::ostream& output){
    output << this->val;
}

BoolVal::BoolVal(bool boolVal){
    this->boolVal = boolVal;
}

bool BoolVal::equals(PTR(Val) other){
    PTR(BoolVal) b = CAST(BoolVal)(other);
    if(b == NULL)
        return false;
    else
        return this->boolVal == b->boolVal;
}

PTR(Val) BoolVal::add_to(PTR(Val) rhs){
    throw std::runtime_error("addition of non-number");
}

PTR(Val) BoolVal::mult_to(PTR(Val) rhs){
    throw std::runtime_error("multiplication of non-number");
}

void BoolVal::print(std::ostream& output){
    if(this->boolVal)
        output << "_true";
    else
        output << "_false";
}

bool BoolVal::is_true(){
    return this->boolVal;
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg){
    throw std::runtime_error("calling not allowed on boolval");
}

void BoolVal::call_step(PTR(Val) actual_arg, PTR(Cont) rest) {
    throw std::runtime_error("attempted to use call_step on a BoolVal");
}

FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env){
    this->formal_arg = formal_arg;
    this->body = body;
    this->env = env;
}

bool FunVal::equals(PTR(Val) other){
    PTR(FunVal) f = CAST(FunVal)(other);
    if(f == NULL)
        return false;
    else
        return (this->formal_arg == f->formal_arg) && (this->body->equals(f->body));
}

PTR(Val) FunVal::add_to(PTR(Val) rhs){
    throw std::runtime_error("addition of non-number");
}

PTR(Val) FunVal::mult_to(PTR(Val) rhs){
    throw std::runtime_error("multiplication of non-number");
}

void FunVal::print(std::ostream& output){
    output << "(_fun (";
    output << this->formal_arg;
    output << ") ";
    this->body->print(output);
    output << ")";
}

bool FunVal::is_true(){
    throw std::runtime_error("Test expression is not a boolean");
}

PTR(Val) FunVal::call(PTR(Val) actual_arg){
    return body->interp(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}

void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formal_arg, actual_arg_val, env);
    Step::cont = rest;
}

TEST_CASE("ValClass"){
    std::string testString = "";
    CHECK((NEW(NumVal)(5))->equals(NEW(NumVal)(5))==true);
    CHECK((NEW(NumVal)(5))->equals(NULL)==false);
    CHECK((NEW(NumVal)(5))->to_string() == "5");
    CHECK_THROWS_WITH((NEW(NumVal)(5))->call(NULL), "calling not allowed on numval");
    CHECK_THROWS_WITH((NEW(NumVal)(5))->add_to(NULL), "add of non-number");
    CHECK_THROWS_WITH((NEW(NumVal)(5))->mult_to(NULL), "mult of non-number");
    CHECK_THROWS_WITH((NEW(NumVal)(5))->is_true(), "Test expression is not a boolean");
    CHECK_THROWS_WITH((NEW(FunVal)("x", NEW(NumExpr)(5), Env::empty))->add_to(NULL), "addition of non-number");
    CHECK_THROWS_WITH((NEW(FunVal)("x", NEW(NumExpr)(5), Env::empty))->mult_to(NULL), "multiplication of non-number");
    CHECK_THROWS_WITH((NEW(FunVal)("x", NEW(NumExpr)(5),Env::empty))->is_true(), "Test expression is not a boolean");
    CHECK_THROWS_WITH((NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("y"), NEW(VarExpr)("x")),Env::empty))->call(NEW(NumVal)(3)), "free variable: y");
    
    CHECK((NEW(BoolVal)(true))->to_string() == "_true");
    CHECK((NEW(BoolVal)(false))->to_string() == "_false");
    CHECK((NEW(BoolVal)(true))->is_true());
    CHECK((NEW(BoolVal)(true))->equals(NULL)==false);
    
    CHECK_THROWS_WITH((NEW(BoolVal)(true))->add_to(NEW(NumVal)(5)), "addition of non-number");
    CHECK_THROWS_WITH((NEW(BoolVal)(true))->mult_to(NEW(NumVal)(5)), "multiplication of non-number");
    CHECK_THROWS_WITH((NEW(BoolVal)(true))->call(NEW(NumVal)(5)), "calling not allowed on boolval");
    
    CHECK((NEW(CallExpr)(NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(8))), NEW(NumExpr)(5)))->interp(Env::empty)->equals(NEW(NumVal)(40)));
    
    PTR(FunExpr)funExp = NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(8)));
    
    CHECK((NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(8))))->equals(funExp) == true);
    CHECK((NEW(FunExpr)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(8))))->equals(NULL) == false);
    
    CHECK((NEW(FunVal)("x", NEW(NumExpr)(5),Env::empty))->equals(NEW(FunVal)("x", NEW(NumExpr)(5),Env::empty))==true);
    CHECK((NEW(FunVal)("x", NEW(NumExpr)(5),Env::empty))->equals(NULL)==false);
    CHECK((NEW(FunVal)("x", NEW(NumExpr)(5),Env::empty))->to_string() == "(_fun (x) 5)");
    CHECK((NEW(FunVal)("x", NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")),Env::empty))->call(NEW(NumVal)(3))->equals(NEW(NumVal)(6))==true);
    CHECK((NEW(FunVal)("x", NEW(NumExpr)(5),Env::empty))->call(NEW(NumVal)(3))->equals(NEW(NumVal)(5))==true);
    CHECK((NEW(FunVal)("x", NEW(NumExpr)(5),Env::empty))->call(NEW(NumVal)(3))->equals(NEW(NumVal)(3))==false);
}
