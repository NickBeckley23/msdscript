//
//  Cont.hpp
//  msdscript
//
//  Created by Nick Beckley on 4/3/21.
//

#ifndef Cont_hpp
#define Cont_hpp

#include <stdio.h>
#include "catch.h"
#include "Parse.h"
#include "pointer.h"
#include "Step.h"

class Expr;
class Env;
class Val;
class VarExpr;
class Cont;

CLASS(Cont) {
public:
    virtual void step_continue() = 0;
    static PTR(Cont) done;
};

class DoneCont : public Cont{
public:
    void step_continue();
};

class RightThenAddCont : public Cont{
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;

    RightThenAddCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class AddCont : public Cont{
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;

    AddCont(PTR(Val) lhs_val, PTR(Cont) rest);

    void step_continue();
};

class RightThenMultCont : public Cont{
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;

    RightThenMultCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class MultCont : public Cont{
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;

    MultCont(PTR(Val) lhs_val, PTR(Cont) rest);

    void step_continue();
};

class IfBranchCont : public Cont{
public:
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    PTR(Env) env;
    PTR(Cont) rest;

    IfBranchCont(PTR(Expr) then_part,  PTR(Expr) else_part, PTR(Env) env, PTR(Cont) rest);

    void step_continue();
};

class LetBodyCont : public Cont{
public:
    std::string lhs;
    PTR(Expr) body;
    PTR(Env) env;
    PTR(Cont) rest;

    LetBodyCont(std::string lhs, PTR(Expr) body, PTR(Env) env, PTR(Cont) cont);
    void step_continue();

};

class RightThenEqCont : public Cont{
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;

    RightThenEqCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class EqCont : public Cont{
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;

    EqCont(PTR(Val) lhs_val, PTR(Cont) rest);

    void step_continue();
};

class ArgThenCallCont : public Cont{
public:
    PTR(Expr) actual_arg;
    PTR(Env) env;
    PTR(Cont) rest;
    ArgThenCallCont(PTR(Expr) actual_arg, PTR(Env) env, PTR(Cont) cont);

    void step_continue();
};

class CallCont : public Cont{
public:
    PTR(Val) to_be_called_val;
    PTR(Cont) rest;

    CallCont(PTR(Val) to_be_called_val, PTR(Cont) rest);
    void step_continue();

};

#endif /* Cont_hpp */
