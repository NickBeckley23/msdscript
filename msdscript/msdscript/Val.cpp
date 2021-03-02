//
//  Val.cpp
//  msdscript
//
//  Created by Nick Beckley on 2/23/21.
//

#include "Val.h"
#include "Expr.h"
#include "catch.h"


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

TEST_CASE("ValClass"){
    std::string testString = "";
    CHECK((new NumVal(5))->equals(new NumVal(5))==true);
    CHECK((new NumVal(5))->equals(NULL)==false);
    CHECK_THROWS_WITH((new NumVal(5))->add_to(NULL), "add of non-number");
    CHECK_THROWS_WITH((new NumVal(5))->mult_to(NULL), "mult of non-number");
    CHECK_THROWS_WITH((new NumVal(5))->is_true(), "Test expression is not a boolean");
    CHECK((new BoolVal(true))->is_true());
    CHECK((new BoolVal(true))->to_expr()->equals(new BoolExpr(true)));
    CHECK((new BoolVal(true))->equals(NULL)==false);
    testString = "5";
    CHECK(((new NumVal(5))->to_expr()->to_string() == "5"));
    CHECK(((new BoolVal(true))->to_expr()->pp_to_string() == "_true"));
    
    CHECK_THROWS_WITH((new BoolVal(true))->add_to(new NumVal(5)), "addition of non-number");
    CHECK_THROWS_WITH((new BoolVal(true))->mult_to(new NumVal(5)), "multiplication of non-number");
    
    
    
}
