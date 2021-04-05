//
//  Expr.hpp
//  msdscript
//
//  Created by Nick Beckley on 1/24/21.
//

#ifndef Expr_h
#define Expr_h

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include "pointer.h"
#include "Env.h"

typedef enum {
    print_group_none,
    print_group_add,
    print_group_add_or_let,
    print_group_add_or_mult_or_let,
    print_group_add_or_eq,
    print_group_eq
} print_mode_t;

class Val;

CLASS(Expr) {
public:
    
    virtual ~Expr() {};
    //checks if 2 expressions are equal
    virtual bool equals(PTR(Expr)other) = 0;
    
    //returns the value of the Expression
    virtual PTR(Val) interp(PTR(Env) env) = 0;
    
    virtual void step_interp() = 0;
    
    //prints simpliest version of epxpression as a string
    virtual void print(std::ostream& output) = 0;
    
    //prints expression with spaces and no extra parenthesis
    virtual void pretty_print(std::ostream& output) = 0;
    
    //takes in a enum print mode to determine the correct format for printing an expression
    virtual void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos) = 0;
    
    //turns expression into a string for easy comparisons
    std::string to_string();
    
    //turns pretty printed expression into string for easy comparison
    std::string pp_to_string();
};
    
class NumExpr : public Expr{
    public:
        int val;
    PTR(Val) numVal;
        
    NumExpr(int val);
    
    bool equals(PTR(Expr)other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};
    
class AddExpr : public Expr {
    public:
        PTR(Expr) lhs;
        PTR(Expr) rhs;
        
    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    
    bool equals(PTR(Expr) other);
    
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};
    
class MultExpr : public Expr {
    public:
        PTR(Expr) lhs;
        PTR(Expr) rhs;
        
    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    
    bool equals(PTR(Expr) other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class VarExpr : public Expr {
    public:
        std::string var;
    
    VarExpr(std::string var);
    
    bool equals(PTR(Expr) other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class LetExpr : public Expr {
    public:
        std::string lhs;
        PTR(Expr) rhs;
        PTR(Expr) body;
    
    LetExpr(std::string lhs, PTR(Expr) rhs, PTR(Expr) body);
    
    bool equals(PTR(Expr)other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
    
};

class BoolExpr : public Expr{
    public:
        bool boolVal;
    PTR(Val) bVal;
        
    BoolExpr(bool boolVal);
    
    bool equals(PTR(Expr)other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class EqExpr : public Expr {
    public:
        PTR(Expr) lhs;
        PTR(Expr) rhs;
        
    EqExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    
    bool equals(PTR(Expr) other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class IfExpr : public Expr {
    public:
    PTR(Expr) test_part;
    PTR(Expr) then_part;
    PTR(Expr) else_part;
        
    IfExpr(PTR(Expr) _if, PTR(Expr) _then, PTR(Expr) _else);
    
    bool equals(PTR(Expr) other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class FunExpr : public Expr {
public:
    std::string formal_arg;
    PTR(Expr) body;
    
    FunExpr(std::string formal_arg, PTR(Expr) body);
    
    bool equals(PTR(Expr) other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class CallExpr : public Expr {
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;
    
    CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);
    
    bool equals(PTR(Expr) other);
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

#endif /* Expr_hpp */
