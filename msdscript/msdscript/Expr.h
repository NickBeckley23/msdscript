//
//  Expr.hpp
//  msdscript
//
//  Created by Nick Beckley on 1/24/21.
//

#ifndef Expr_hpp
#define Expr_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>


typedef enum {
    print_group_none,
    print_group_add,
    print_group_add_or_let,
    print_group_add_or_mult_or_let,
    print_group_add_or_eq,
    print_group_eq
} print_mode_t;

class Val;

class Expr {
public:
    
    //checks if 2 expressions are equal
    virtual bool equals(Expr *other) = 0;
    
    //returns the value of the Expression
    virtual Val* interp() = 0;
    
    //substitutes a given string for an expression
    virtual Expr* subst(std::string string, Expr *exp) = 0;
    
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
        
    NumExpr(int val);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};
    
class AddExpr : public Expr {
    public:
        Expr *lhs;
        Expr *rhs;
        
    AddExpr(Expr *lhs, Expr *rhs);
    
    bool equals(Expr *other);
    
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
    };
    
class MultExpr : public Expr {
    public:
        Expr *lhs;
        Expr *rhs;
        
    MultExpr(Expr *lhs, Expr *rhs);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
    };

class VarExpr : public Expr {
    public:
        std::string var;
    
    VarExpr(std::string var);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class LetExpr : public Expr {
    public:
        std::string lhs;
        Expr* rhs;
        Expr* body;
    
    LetExpr(std::string lhs, Expr* rhs, Expr* body);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr* exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
    
};

class BoolExpr : public Expr{
    public:
        bool boolVal;
        
    BoolExpr(bool boolVal);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class EqExpr : public Expr {
    public:
        Expr *lhs;
        Expr *rhs;
        
    EqExpr(Expr *lhs, Expr *rhs);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
    };

class IfExpr : public Expr {
    public:
    Expr *test_part;
    Expr *then_part;
    Expr *else_part;
        
    IfExpr(Expr *_if, Expr *_then, Expr *_else);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
    };

class FunExpr : public Expr {
public:
    std::string formal_arg;
    Expr *body;
    
    FunExpr(std::string formal_arg, Expr *body);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
};

class CallExpr : public Expr {
public:
    Expr *to_be_called;
    Expr *actual_arg;
    
    CallExpr(Expr *to_be_called, Expr *actual_arg);
    
    bool equals(Expr *other);
    Val* interp();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long *pos);
    
};

#endif /* Expr_hpp */
