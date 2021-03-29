//
//  Val.hpp
//  msdscript
//
//  Created by Nick Beckley on 2/23/21.
//

#ifndef Val_h
#define Val_h

#include <stdio.h>
#include <iostream>
#include "pointer.h"
#include "Env.h"

class Expr;

CLASS(Val) {
public:
    virtual ~Val() {};
//    virtual PTR(Expr) to_expr() = 0;
    virtual bool equals(PTR(Val) v) = 0;
    virtual PTR(Val) add_to(PTR(Val) rhs) = 0;
    virtual PTR(Val) mult_to(PTR(Val) rhs) = 0;
    virtual void print(std::ostream& output) = 0;
    virtual bool is_true() = 0;
    virtual PTR(Val) call(PTR(Val) actual_arg) = 0;
    std::string to_string();
};

class NumVal : public Val {
public:
    int val;
    
    NumVal(int val);
    
//    PTR(Expr) to_expr();
    bool equals(PTR(Val) v);
    PTR(Val) add_to(PTR(Val) rhs);
    PTR(Val) mult_to(PTR(Val) rhs);
    void print(std::ostream& output);
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    
};

class BoolVal : public Val {
public:
    bool boolVal;
    
    BoolVal(bool boolVal);
    
//    PTR(Expr) to_expr();
    bool equals(PTR(Val) v);
    PTR(Val) add_to(PTR(Val) rhs);
    PTR(Val) mult_to(PTR(Val) rhs);
    void print(std::ostream& output);
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    
};

class FunVal : public Val {
public:
    std::string formal_arg;
    PTR(Expr)body;
    PTR(Env) env;
    
    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);
    
//    PTR(Expr) to_expr();
    bool equals(PTR(Val) v);
    PTR(Val) add_to(PTR(Val) rhs);
    PTR(Val) mult_to(PTR(Val) rhs);
    void print(std::ostream& output);
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
};

#endif /* Val_hpp */
