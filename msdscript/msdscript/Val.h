//
//  Val.hpp
//  msdscript
//
//  Created by Nick Beckley on 2/23/21.
//

#ifndef Val_hpp
#define Val_hpp

#include <stdio.h>
#include <iostream>

class Expr;

class Val{
public:
    virtual Expr* to_expr() = 0;
    virtual bool equals(Val* v) = 0;
    virtual Val* add_to(Val* rhs) = 0;
    virtual Val* mult_to(Val* rhs) = 0;
    virtual void print(std::ostream& output) = 0;
    
};

class NumVal : public Val {
public:
    int val;
    
    NumVal(int val);
    
    Expr* to_expr();
    bool equals(Val* v);
    Val* add_to(Val* rhs);
    Val* mult_to(Val* rhs);
    void print(std::ostream& output);
    
};

#endif /* Val_hpp */
