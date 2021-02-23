//
//  Val.cpp
//  msdscript
//
//  Created by Nick Beckley on 2/23/21.
//

#include "Val.h"
#include "Expr.h"


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
        throw std::runtime_error("add of non-number");
    return new NumVal(this->val * other_num->val);
}

void NumVal::print(std::ostream& output){
    output << this->val;
}
