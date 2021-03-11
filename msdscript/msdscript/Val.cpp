//
//  Val.cpp
//  msdscript
//
//  Created by Nick Beckley on 2/23/21.
//

#include "Val.h"
#include "Expr.h"
#include "catch.h"

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

Expr* NumVal::to_expr(){
    return new NumExpr(this->val);
}

bool NumVal::equals(Val* other){
    NumVal *num = dynamic_cast<NumVal*>(other);
    if(num == NULL)
        return false;
    else
        return this->val == num->val;
}

Val* NumVal::add_to(Val* rhs){
    NumVal *other_num = dynamic_cast<NumVal*>(rhs);
    if(other_num == NULL)
        throw std::runtime_error("add of non-number");
    return new NumVal(this->val + other_num->val);
}

Val* NumVal::mult_to(Val* rhs){
    NumVal *other_num = dynamic_cast<NumVal*>(rhs);
    if(other_num == NULL)
        throw std::runtime_error("mult of non-number");
    return new NumVal(this->val * other_num->val);
}

bool NumVal::is_true(){
    throw std::runtime_error("Test expression is not a boolean");
}

Val* NumVal::call(Val* actual_arg){
    throw std::runtime_error("calling not allowed on numval");
}

void NumVal::print(std::ostream& output){
    output << this->val;
}

BoolVal::BoolVal(bool boolVal){
    this->boolVal = boolVal;
}

Expr* BoolVal::to_expr(){
    return new BoolExpr(this->boolVal);
}

bool BoolVal::equals(Val* other){
    BoolVal *b = dynamic_cast<BoolVal*>(other);
    if(b == NULL)
        return false;
    else
        return this->boolVal == b->boolVal;
}

Val* BoolVal::add_to(Val* rhs){
    throw std::runtime_error("addition of non-number");
}

Val* BoolVal::mult_to(Val* rhs){
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

Val* BoolVal::call(Val *actual_arg){
    throw std::runtime_error("calling not allowed on boolval");
}

FunVal::FunVal(std::string formal_arg, Expr* body){
    this->formal_arg = formal_arg;
    this->body = body;
}

Expr* FunVal::to_expr(){
    return new FunExpr(this->formal_arg, this->body);
}

bool FunVal::equals(Val* other){
    FunVal *f = dynamic_cast<FunVal*>(other);
    if(f == NULL)
        return false;
    else
        return (this->formal_arg == f->formal_arg) && (this->body->equals(f->body));
}

Val* FunVal::add_to(Val* rhs){
    throw std::runtime_error("addition of non-number");
}

Val* FunVal::mult_to(Val* rhs){
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

Val* FunVal::call(Val *actual_arg){
    return body->subst(formal_arg, actual_arg->to_expr())->interp();
}

TEST_CASE("ValClass"){
    std::string testString = "";
    CHECK((new NumVal(5))->equals(new NumVal(5))==true);
    CHECK((new NumVal(5))->equals(NULL)==false);
    CHECK((new NumVal(5))->to_string() == "5");
    CHECK_THROWS_WITH((new NumVal(5))->call(NULL), "calling not allowed on numval");
    CHECK_THROWS_WITH((new NumVal(5))->add_to(NULL), "add of non-number");
    CHECK_THROWS_WITH((new NumVal(5))->mult_to(NULL), "mult of non-number");
    CHECK_THROWS_WITH((new NumVal(5))->is_true(), "Test expression is not a boolean");
    CHECK_THROWS_WITH((new FunVal("x", new NumExpr(5)))->add_to(NULL), "addition of non-number");
    CHECK_THROWS_WITH((new FunVal("x", new NumExpr(5)))->mult_to(NULL), "multiplication of non-number");
    CHECK_THROWS_WITH((new FunVal("x", new NumExpr(5)))->is_true(), "Test expression is not a boolean");
    CHECK((new BoolVal(true))->to_string() == "_true");
    CHECK((new BoolVal(false))->to_string() == "_false");
    CHECK((new BoolVal(true))->is_true());
    CHECK((new BoolVal(true))->to_expr()->equals(new BoolExpr(true)));
    CHECK((new BoolVal(true))->equals(NULL)==false);
    testString = "5";
    CHECK(((new NumVal(5))->to_expr()->to_string() == "5"));
    CHECK(((new BoolVal(true))->to_expr()->pp_to_string() == "_true"));
    
    CHECK_THROWS_WITH((new BoolVal(true))->add_to(new NumVal(5)), "addition of non-number");
    CHECK_THROWS_WITH((new BoolVal(true))->mult_to(new NumVal(5)), "multiplication of non-number");
    CHECK_THROWS_WITH((new BoolVal(true))->call(new NumVal(5)), "calling not allowed on boolval");
    
    CHECK((new CallExpr(new FunExpr("x", new MultExpr(new VarExpr("x"), new NumExpr(8))), new NumExpr(5)))->interp()->equals(new NumVal(40)));
    
    FunExpr *funExp = new FunExpr("x", new MultExpr(new VarExpr("x"), new NumExpr(8)));
    
    CHECK((new FunExpr("x", new MultExpr(new VarExpr("x"), new NumExpr(8))))->equals(funExp) == true);
    CHECK((new FunExpr("x", new MultExpr(new VarExpr("x"), new NumExpr(8))))->equals(NULL) == false);
    
    CHECK((new FunVal("x", new NumExpr(5)))->equals(new FunVal("x", new NumExpr(5)))==true);
    CHECK((new FunVal("x", new NumExpr(5)))->equals(NULL)==false);
    CHECK((new FunVal("x", new NumExpr(5)))->to_string() == "(_fun (x) 5)");
    
    
}
