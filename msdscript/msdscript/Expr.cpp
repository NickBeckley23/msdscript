//
//  Expr.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/24/21.
//

#include "Expr.h"
#include "catch.h"
#include <stdexcept>

Num::Num(int val) {
    this->val = val;
};

bool Num::equals(Expr *other){
    Num *num = dynamic_cast<Num*>(other);
    if(num == NULL)
        return false;
    else
        return this->val == num->val;
};

int Num::interp(){
    return this->val;
};

bool Num::has_variable(){
    return false;
};

Expr* Num::subst(std::string string,Expr *exp){
    return this;
};
    
Add::Add(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
};

bool Add::equals(Expr *other){
    Add *o = dynamic_cast<Add*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
};

int Add::interp(){
    return this->lhs->interp() + this->rhs->interp();
};

bool Add::has_variable(){
    return (this->lhs->has_variable() || this->rhs->has_variable());
};

Expr* Add::subst(std::string string, Expr *exp){
    return new Add(lhs->subst(string, exp),rhs->subst(string, exp));
};
    
Mult::Mult(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
};

bool Mult::equals(Expr *other){
    Mult *o = dynamic_cast<Mult*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
};

int Mult::interp(){
    return this->lhs->interp() * this->rhs->interp();
};

bool Mult::has_variable(){
    return (this->lhs->has_variable() || this->rhs->has_variable());
};

Expr* Mult::subst(std::string string, Expr *exp){
    return new Mult(lhs->subst(string, exp), rhs->subst(string, exp));
}

Var::Var(std::string var){
    this->var = var;
};

bool Var::equals(Expr *other){
    Var *o = dynamic_cast<Var*>(other);
    if(o == NULL)
        return false;
    else
        return(this->var == o->var);
};

int Var::interp(){
    throw std::runtime_error("variable has no value");
}

bool Var::has_variable(){
    return true;
};

Expr* Var::subst(std::string string, Expr *exp){
    if(this->var == string)
        return exp;
    else
        return this;
};


TEST_CASE("equals"){
    
    Num *num1 = new Num(1);
    Num *num2 = new Num(2);
    Num *num3 = new Num(3);
    Num *num4 = new Num(4);
    
    CHECK((new Add(new Num(5), new Num(4)))->equals(new Add(new Num(5), new Num(4))) == true);
    CHECK((new Add(num1, num2))->equals(new Add(num1, num2)) == true);
    CHECK((new Add(num3, num4))->equals(new Add(num1, num2)) == false);
    CHECK((new Mult(num1, num2))->equals(new Mult(num1, num2)) == true);
    CHECK((new Mult(num3, num4))->equals(new Mult(num1, num2)) == false);
    CHECK((new Num(5))->equals(new Num(5)) == true);
    CHECK((new Num(0))->equals(new Num(5)) == false);
    CHECK((new Var("hi"))->equals(new Var("hi")) == true);
    CHECK((new Var("ew"))->equals(new Var("hi")) == false);
    CHECK_THROWS_WITH((new Var("x"))->interp(), "variable has no value");
    CHECK( (new Add(new Var("x"), new Num(7)))
           ->subst("x", new Var("y"))
           ->equals(new Add(new Var("y"), new Num(7))) );
    CHECK((new Num(8))->subst("x", new Num(8))->equals(new Num(8)));
    
};
