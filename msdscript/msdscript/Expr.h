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
  print_group_add_or_mult,
    print_group_let
} print_mode_t;


class Expr {
public:
    
    //checks if 2 expressions are equal
    virtual bool equals(Expr *other) = 0;
    
    //returns the value of the Expression
    virtual int interp() = 0;
    
    //checks if there is a variable in the expression
    virtual bool has_variable() = 0;
    
    //substitutes a given string for an expression
    virtual Expr* subst(std::string string, Expr *exp) = 0;
    
    //prints simpliest version of epxpression as a string
    virtual void print(std::ostream& output) = 0;
    
    //prints expression with spaces and no extra parenthesis
    virtual void pretty_print(std::ostream& output) = 0;
    
    //takes in a enum print mode to determine the correct format for printing an expression
    virtual void pretty_print_at(std::ostream& output, print_mode_t mode, long pos) = 0;
    
    //turns expression into a string for easy comparisons
    std::string to_string();
    
    //turns pretty printed expression into string for easy comparison
    std::string pp_to_string();
};
    
class Num : public Expr{
    public:
        int val;
        
    Num(int val);
    
    bool equals(Expr *other);
    int interp();
    bool has_variable();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long pos);
};
    
class Add : public Expr {
    public:
        Expr *lhs;
        Expr *rhs;
        
    Add(Expr *lhs, Expr *rhs);
    
    bool equals(Expr *other);
    
    int interp();
    bool has_variable();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long pos);
    };
    
class Mult : public Expr {
    public:
        Expr *lhs;
        Expr *rhs;
        
    Mult(Expr *lhs, Expr *rhs);
    
    bool equals(Expr *other);
    int interp();
    bool has_variable();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long pos);
    };

class Var : public Expr {
    public:
        std::string var;
    
    Var(std::string var);
    
    bool equals(Expr *other);
    int interp();
    bool has_variable();
    Expr* subst(std::string string, Expr *exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long pos);
};

class Let : public Expr {
    public:
        std::string lhs;
        Expr* rhs;
        Expr* body;
    
    Let(std::string lhs, Expr* rhs, Expr* body);
    
    bool equals(Expr *other);
    int interp();
    bool has_variable();
    Expr* subst(std::string string, Expr* exp);
    void print(std::ostream& output);
    void pretty_print(std::ostream& output);
    void pretty_print_at(std::ostream& output, print_mode_t mode, long pos);
    
};
#endif /* Expr_hpp */
