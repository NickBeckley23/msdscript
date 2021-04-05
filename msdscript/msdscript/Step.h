//
//  Step.hpp
//  msdscript
//
//  Created by Nick Beckley on 4/3/21.
//

#ifndef Step_hpp
#define Step_hpp

#include <stdio.h>
#include "Expr.h"
#include "catch.h"
#include "Parse.h"
#include "Val.h"
#include "pointer.h"
#include "Env.h"
#include "Cont.h"

class Expr;
class Env;
class Val;
class VarExpr;
class Cont;

class Step {
public:
    typedef enum {
        interp_mode,
        continue_mode
    } mode_t;
    
    static mode_t mode;
    
    static PTR(Expr) expr;
    static PTR(Env) env;
    static PTR(Val) val;
    static PTR(Cont) cont;
    static PTR(Val) interp_by_steps(PTR(Expr) e);
    
};

#endif /* Step_hpp */
