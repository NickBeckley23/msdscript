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
};
    
class Num : public Expr{
    public:
        int val;
        
    Num(int val);
    
    bool equals(Expr *other);
    int interp();
    bool has_variable();
    Expr* subst(std::string string, Expr *exp);
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
    };

class Var : public Expr {
    public:
        std::string var;
    
    Var(std::string var);
    
    bool equals(Expr *val);
    int interp();
    bool has_variable();
    Expr* subst(std::string string, Expr *exp);
};

#endif /* Expr_hpp */
