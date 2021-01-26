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
    virtual bool equals(Expr *other) = 0;
};
    
class Num : public Expr{
    public:
        int val;
        
    Num(int val);
    
    virtual bool equals(Expr *other);
};
    
class Add : public Expr {
    public:
        Expr *lhs;
        Expr *rhs;
        
    Add(Expr *lhs, Expr *rhs);
    
    virtual bool equals(Expr *other);
    };
    
class Mult : public Expr {
    public:
        Expr *lhs;
        Expr *rhs;
        
    Mult(Expr *lhs, Expr *rhs);
    
    virtual bool equals(Expr *other);
    };

class Variable : public Expr {
    public:
        std::string var;
    
    Variable(std::string var);
    
    virtual bool equals(Expr *val);
};

#endif /* Expr_hpp */
