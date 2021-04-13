//
//  Expr.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/24/21.
//

#include "Expr.h"
#include "catch.h"
#include "Parse.h"
#include "Val.h"
#include "Step.h"
#include "Cont.h"
#include <stdexcept>

std::string Expr::to_string(){
    std::ostream output(nullptr);
    std::stringbuf strBuf;
    output.rdbuf(&strBuf);
    print(output);
    return strBuf.str();
}

std::string Expr::pp_to_string(){
    std::ostream output(nullptr);
    std::stringbuf strBuf;
    output.rdbuf(&strBuf);
    pretty_print(output);
    return strBuf.str();
}

NumExpr::NumExpr(int val) {
    this->numVal = NEW(NumVal)(val);
    this->val = val;
}

bool NumExpr::equals(PTR(Expr) other){
    PTR(NumExpr) num = CAST(NumExpr)(other);
    if(num == NULL)
        return false;
    else
        return this->val == num->val;
}

PTR(Val) NumExpr::interp(PTR(Env) env){
    return numVal;
}

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = numVal;
}

void NumExpr::print(std::ostream& output){
    output << this->val;
}

void NumExpr::pretty_print(std::ostream& output){
    this->pretty_print_at(output, print_group_none, 0);
}

void NumExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    output << this->val;
}

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) other){
    PTR(AddExpr) o = CAST(AddExpr)(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
}

PTR(Val) AddExpr::interp(PTR(Env) env){
    return this->lhs->interp(env)->add_to(this->rhs->interp(env));
}

void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}

void AddExpr::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "+";
    this->rhs->print(output);
    output << ")";
}

void AddExpr::pretty_print(std::ostream& output){
    long pos = 0;
    long *positionPtr = &pos;
    lhs->pretty_print_at(output, print_group_add_or_let, positionPtr);
    output << " + ";
    rhs->pretty_print_at(output, print_group_add_or_eq, positionPtr);
}

void AddExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    if(mode == print_group_add_or_let || mode == print_group_add || mode == print_group_add_or_mult_or_let)
        output << "(";
    lhs->pretty_print_at(output, print_group_add_or_let, pos);
    output << " + ";
    rhs->pretty_print_at(output, print_group_add_or_eq, pos);
    if(mode == print_group_add_or_let || mode == print_group_add || mode == print_group_add_or_mult_or_let)
        output << ")";
}

MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) other){
    PTR(MultExpr) o = CAST(MultExpr)(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
}

PTR(Val) MultExpr::interp(PTR(Env) env){
    return this->lhs->interp(env)->mult_to(this->rhs->interp(env));
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}

void MultExpr::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "*";
    this->rhs->print(output);
    output << ")";
}

void MultExpr::pretty_print(std::ostream& output){
    long pos = 0;
    long *positionPtr = &pos;
    lhs->pretty_print_at(output, print_group_add_or_mult_or_let, positionPtr);
    output << " * ";
    rhs->pretty_print_at(output, print_group_add, positionPtr);
}

void MultExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    if(mode == print_group_add_or_mult_or_let){
        output << "(";
        lhs->pretty_print_at(output, print_group_add_or_mult_or_let, pos);
        output << " * ";
        rhs->pretty_print_at(output, print_group_add, pos);
        output << ")";
    }else if(mode == print_group_add_or_let){
        lhs->pretty_print_at(output, print_group_add_or_mult_or_let, pos);
        output << " * ";
        rhs->pretty_print_at(output, print_group_add_or_let, pos);
    }else{
        lhs->pretty_print_at(output, print_group_add_or_mult_or_let, pos);
        output << " * ";
        rhs->pretty_print_at(output, print_group_add, pos);
    }
}

VarExpr::VarExpr(std::string var){
    this->var = var;
}

bool VarExpr::equals(PTR(Expr) other){
    PTR(VarExpr) o = CAST(VarExpr)(other);
    if(o == NULL)
        return false;
    else
        return(this->var == o->var);
}

PTR(Val) VarExpr::interp(PTR(Env) env){
    return env->lookup(var);
}

void VarExpr::step_interp() {
    Step::val = Step::env->lookup(var);
    Step::mode = Step::continue_mode;
}

void VarExpr::print(std::ostream& output){
    output << this->var;
}

void VarExpr::pretty_print(std::ostream& output){
    output << this->var;
}

void VarExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    output << this->var;
}

LetExpr::LetExpr(std::string lhs, PTR(Expr) rhs, PTR(Expr) body){
    this->lhs = lhs;
    this->rhs = rhs;
    this->body = body;
}

bool LetExpr::equals(PTR(Expr) other){
    PTR(LetExpr) o = CAST(LetExpr)(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs == o->lhs && this->rhs->equals(o->rhs) && this->body->equals(o->body));
}

PTR(Val) LetExpr::interp(PTR(Env) env){
    PTR(Val) n = this->rhs->interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(lhs, n, env);
    return this->body->interp(new_env);
}

void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::cont = NEW(LetBodyCont)(lhs, body, Step::env, Step::cont);
}

void LetExpr::print(std::ostream& output){
    output << "(_let ";
    output << lhs;
    output << "=";
    rhs->print(output);
    output << " _in ";
    body->print(output);
    output << ")";
}

void LetExpr::pretty_print(std::ostream& output){
    long pos = 0;
    long *positionPtr = &pos;
    output << "_let " << this->lhs << " = ";
    this->rhs->pretty_print_at(output, print_group_none, positionPtr);
    output << "\n";
    *positionPtr = output.tellp();
    output << "_in  ";
    this->body->pretty_print_at(output, print_group_none, positionPtr);
    
}
void LetExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    if(mode == print_group_eq || mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
        output << "(";
    long letPos = output.tellp();
    long spaces = letPos - *pos;
    output << "_let ";
    output << this->lhs << " = ";
    this->rhs->pretty_print_at(output, print_group_none, pos);
    output << "\n";
    *pos = output.tellp();
    for(int i = 0; i < spaces; i++){
        output << " ";
    }
    output << "_in  ";
    this->body->pretty_print_at(output, print_group_none, pos);
    if(mode == print_group_eq ||mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
        output << ")";
}

BoolExpr::BoolExpr(bool boolVal) {
    this->bVal = NEW(BoolVal)(boolVal);
    this->boolVal = boolVal;
}

bool BoolExpr::equals(PTR(Expr) other){
    PTR(BoolExpr) b = CAST(BoolExpr)(other);
    if(b == NULL)
        return false;
    else
        return this->boolVal == b->boolVal;
}

PTR(Val) BoolExpr::interp(PTR(Env) env){
    return bVal;
}

void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = bVal;
}

void BoolExpr::print(std::ostream& output){
    if(this->boolVal)
        output << "_true";
    else
        output << "_false";
}

void BoolExpr::pretty_print(std::ostream& output){
    pretty_print_at(output, print_group_none, 0);
}

void BoolExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    print(output);
}

EqExpr::EqExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool EqExpr::equals(PTR(Expr) other){
    PTR(EqExpr) e = CAST(EqExpr)(other);
    if(e == NULL)
        return false;
    else
        return (this->lhs->equals(e->lhs) && this->rhs->equals(e->rhs));
}

PTR(Val) EqExpr::interp(PTR(Env) env){
    return NEW(BoolVal)(lhs->interp(env)->equals(rhs->interp(env)));
}

void EqExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    //Step::env = Step::env; no-op
    Step::cont = NEW(RightThenEqCont)(rhs, Step::env, Step::cont);
}

void EqExpr::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "==";
    this->rhs->print(output);
    output << ")";
}

void EqExpr::pretty_print(std::ostream& output){
    long pos = 0;
    long *positionPtr = &pos;
    lhs->pretty_print_at(output, print_group_eq, positionPtr);
    output << " == ";
    rhs->pretty_print_at(output, print_group_none, positionPtr);
}

void EqExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    if(mode == print_group_none){
        lhs->pretty_print_at(output, print_group_eq, pos);
        output << " == ";
        rhs->pretty_print_at(output, print_group_none, pos);
    }else{
        output << "(";
        lhs->pretty_print_at(output, print_group_eq, pos);
        output << " == ";
        rhs->pretty_print_at(output, print_group_none, pos);
        output << ")";
    }
}

IfExpr::IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part){
    this->test_part = test_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) other){
    PTR(IfExpr) o = CAST(IfExpr)(other);
    if(o == NULL)
        return false;
    else
        return (this->test_part->equals(o->test_part) && this->then_part->equals(o->then_part) && this->else_part->equals(o->else_part));
}

PTR(Val) IfExpr::interp(PTR(Env) env){
    if(test_part->interp(env)->is_true())
        return then_part->interp(env);
    else
        return else_part->interp(env);
}

void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = test_part;
    Step::cont = NEW(IfBranchCont)(then_part, else_part, Step::env, Step::cont);
}

void IfExpr::print(std::ostream& output){
    output << "(_if ";
    this->test_part->print(output);
    output << " _then ";
    this->then_part->print(output);
    output << " _else ";
    this->else_part->print(output);
    output << ")";
}

void IfExpr::pretty_print(std::ostream& output){
    long pos = 0;
    long *positionPtr = &pos;
    output << "_if ";
    this->test_part->pretty_print_at(output, print_group_none, positionPtr);
    output << "\n";
    *positionPtr = output.tellp();
    output << "_then ";
    this->then_part->pretty_print_at(output, print_group_none, positionPtr);
    output << "\n";
    *positionPtr = output.tellp();
    output << "_else ";
    this->else_part->pretty_print_at(output, print_group_none, positionPtr);
}
void IfExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    if(mode == print_group_eq || mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
        output << "(";
    long letPos = output.tellp();
    long spaces = letPos - *pos;
    output << "_if ";
    this->test_part->pretty_print_at(output, print_group_none, pos);
    output << "\n";
    *pos = output.tellp();
    for(int i = 0; i < spaces; i++){
        output << " ";
    }
    output << "_then ";
    this->then_part->pretty_print_at(output, print_group_none, pos);
    output << "\n";
    *pos = output.tellp();
    for(int i = 0; i < spaces; i++){
        output << " ";
    }
    output << "_else ";
    this->else_part->pretty_print_at(output, print_group_none, pos);
    if(mode == print_group_eq || mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
        output << ")";
}

FunExpr::FunExpr(std::string formal_arg, PTR(Expr) body){
    this->formal_arg = formal_arg;
    this->body = body;
}

bool FunExpr::equals(PTR(Expr) other){
    PTR(FunExpr) o = CAST(FunExpr)(other);
    if(o == NULL)
        return false;
    else
        return (this->formal_arg == o->formal_arg) && this->body->equals(o->body);
}

PTR(Val) FunExpr::interp(PTR(Env) env){
    return NEW(FunVal)(this->formal_arg, this->body, env);
}

void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, body, Step::env);
}

//PTR(Expr) FunExpr::subst(std::string string, PTR(Expr) exp){
//    if(this->formal_arg != string){
//        PTR(Expr)new_body = body->subst(string, exp);
//        return NEW(FunExpr)(this->formal_arg, new_body);
//    }
//    return THIS;
//}

void FunExpr::print(std::ostream& output){
    output << "(_fun (";
    output << this->formal_arg;
    output << ") ";
    this->body->print(output);
    output << ")";
}

void FunExpr::pretty_print(std::ostream& output){
//    long pos = 0;
//    long *positionPtr = &pos;
//    output << "_if ";
//    this->test_part->pretty_print_at(output, print_group_none, positionPtr);
//    output << "\n";
//    *positionPtr = output.tellp();
//    output << "_then ";
//    this->then_part->pretty_print_at(output, print_group_none, positionPtr);
//    output << "\n";
//    *positionPtr = output.tellp();
//    output << "_else ";
//    this->else_part->pretty_print_at(output, print_group_none, positionPtr);
}
void FunExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
//    if(mode == print_group_eq || mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
//        output << "(";
//    long letPos = output.tellp();
//    long spaces = letPos - *pos;
//    output << "_if ";
//    this->test_part->pretty_print_at(output, print_group_none, pos);
//    output << "\n";
//    *pos = output.tellp();
//    for(int i = 0; i < spaces; i++){
//        output << " ";
//    }
//    output << "_then ";
//    this->then_part->pretty_print_at(output, print_group_none, pos);
//    output << "\n";
//    *pos = output.tellp();
//    for(int i = 0; i < spaces; i++){
//        output << " ";
//    }
//    output << "_else ";
//    this->else_part->pretty_print_at(output, print_group_none, pos);
//    if(mode == print_group_eq || mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
//        output << ")";
}

CallExpr::CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg){
    this->to_be_called = to_be_called;
    this->actual_arg = actual_arg;
}

bool CallExpr::equals(PTR(Expr) other){
    PTR(CallExpr) c = CAST(CallExpr)(other);
    if(c == NULL)
        return false;
    else
        return (this->to_be_called->equals(c->to_be_called) && this->actual_arg->equals(c->actual_arg));
}

PTR(Val) CallExpr::interp(PTR(Env) env){
    return this->to_be_called->interp(env)->call(this->actual_arg->interp(env));
}

void CallExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}

void CallExpr::print(std::ostream& output){
    this->to_be_called->print(output);
      output << "(";
      this->actual_arg->print(output);
      output << ")";
}

void CallExpr::pretty_print(std::ostream& output){
//    long pos = 0;
//    long *positionPtr = &pos;
//    output << "_if ";
//    this->test_part->pretty_print_at(output, print_group_none, positionPtr);
//    output << "\n";
//    *positionPtr = output.tellp();
//    output << "_then ";
//    this->then_part->pretty_print_at(output, print_group_none, positionPtr);
//    output << "\n";
//    *positionPtr = output.tellp();
//    output << "_else ";
//    this->else_part->pretty_print_at(output, print_group_none, positionPtr);
}
void CallExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
//    if(mode == print_group_eq || mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
//        output << "(";
//    long letPos = output.tellp();
//    long spaces = letPos - *pos;
//    output << "_if ";
//    this->test_part->pretty_print_at(output, print_group_none, pos);
//    output << "\n";
//    *pos = output.tellp();
//    for(int i = 0; i < spaces; i++){
//        output << " ";
//    }
//    output << "_then ";
//    this->then_part->pretty_print_at(output, print_group_none, pos);
//    output << "\n";
//    *pos = output.tellp();
//    for(int i = 0; i < spaces; i++){
//        output << " ";
//    }
//    output << "_else ";
//    this->else_part->pretty_print_at(output, print_group_none, pos);
//    if(mode == print_group_eq || mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
//        output << ")";
}

TEST_CASE("Expression Tests"){
    std::stringstream ss;
    PTR(NumExpr)num1 = NEW(NumExpr)(1);
    PTR(NumExpr)num2 = NEW(NumExpr)(2);
    PTR(NumExpr)num3 = NEW(NumExpr)(3);
    PTR(NumExpr)num4 = NEW(NumExpr)(4);
    PTR(VarExpr)myVar = NEW(VarExpr)("x");
    PTR(NumExpr)nullNum = NULL;
    nullNum = NULL;
    PTR(LetExpr)nullLet = NULL;

    PTR(LetExpr)let1 = NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)(NEW(NumExpr)(5), NEW(VarExpr)("x")));
    PTR(LetExpr)let2 = NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)(NEW(NumExpr)(5), NEW(VarExpr)("x")));
    std::string pp_let1 = "_let x = 5\n        _in  (_let y = 3\n              _in  y + 2) + x";

    CHECK((NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(5), NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(VarExpr)("x"))), NEW(NumExpr) (1)))->interp(Env::empty)->equals(NEW(NumVal)(26)));
    CHECK((NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(5), NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(VarExpr)("x"))), NEW(NumExpr) (1)))->pp_to_string() == "5 * (_let x = 5\n     _in  x) + 1");

    CHECK((NEW(MultExpr)(NEW(NumExpr)(5), NEW(AddExpr)(NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(VarExpr)("x")), NEW(NumExpr) (1))))->interp(Env::empty)->equals(NEW(NumVal)(30)));
    CHECK((NEW(MultExpr)(NEW(NumExpr)(5), NEW(AddExpr)(NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(VarExpr)("x")), NEW(NumExpr) (1))))->pp_to_string() == "5 * ((_let x = 5\n      _in  x) + 1)");

    CHECK((NEW(MultExpr)(NEW(NumExpr)(5), NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr) (1)))))->pp_to_string() == "5 * _let x = 5\n    _in  x + 1");
    CHECK((NEW(MultExpr)(NEW(NumExpr)(5), NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr) (1)))))->interp(Env::empty)->equals(NEW(NumVal)(30)));

    CHECK ((NEW(MultExpr)( NEW(NumExpr) (5), (NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(LetExpr)("y", NEW(NumExpr)(3), NEW(LetExpr)( "z", NEW(NumExpr)(1), NEW(AddExpr) (NEW(VarExpr)("z"), NEW(NumExpr)(4))))))))->pp_to_string() == "5 * _let x = 5\n    _in  _let y = 3\n         _in  _let z = 1\n              _in  z + 4");
    //Let pretty print
    CHECK ((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(LetExpr)("x", NEW(NumExpr)(1), NEW(AddExpr)( NEW(VarExpr)("x"), NEW(NumExpr)(3)))))->pp_to_string() == "_let x = 5\n_in  _let x = 1\n     _in  x + 3");

    pp_let1 = "_let x = 5\n_in  x + 1";
    CHECK ((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))->pp_to_string() == pp_let1);

    pp_let1 = "_let x = 5\n_in  _let x = 2\n     _in  x + 3";
    CHECK ((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)( NEW(VarExpr)("x"), NEW(NumExpr)(3)))))->pp_to_string()== pp_let1);

    pp_let1 = "_let x = 5\n_in  (_let y = 3\n      _in  y + 2) + x";
    CHECK ((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(LetExpr)("y", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(2))), NEW(VarExpr)("x"))))
           ->pp_to_string() == pp_let1);


    CHECK((NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(3)))->pp_to_string() == "0 == 3");

    CHECK((NEW(AddExpr) (NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(3)), NEW(NumExpr)(6)))->pp_to_string() == "(0 == 3) + 6");
    CHECK((NEW(MultExpr) (NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(3)), NEW(NumExpr)(6)))->pp_to_string() == "(0 == 3) * 6");
    CHECK((NEW(AddExpr) (NEW(NumExpr)(6), NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(3))))->pp_to_string() == "6 + (0 == 3)");
    CHECK((NEW(MultExpr) (NEW(EqExpr)(NEW(NumExpr)(7), NEW(NumExpr)(4)), NEW(NumExpr)(6)))->pp_to_string() == "(7 == 4) * 6");
    CHECK((NEW(MultExpr) (NEW(NumExpr)(6), NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(3))))->pp_to_string() == "6 * (0 == 3)");
    CHECK((NEW(EqExpr)(NEW(EqExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)), NEW(EqExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4))))->pp_to_string() == "(1 == 2) == 3 == 4");

    CHECK((NEW(EqExpr)(NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(VarExpr)("x")), NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(VarExpr)("x"))))->pp_to_string() == "(_let x = 5\n _in  x) == _let x = 5\n            _in  x");

    CHECK((NEW(EqExpr)(NEW(LetExpr)("x", NEW(NumExpr)(5),NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))),NEW(LetExpr)("x",NEW(NumExpr)(3),NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)))))->pp_to_string() == "(_let x = 5\n _in  x + 3) == _let x = 3\n                _in  x + 5");

    CHECK((NEW(LetExpr)("x", NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(3)), NEW(VarExpr)("x")))->pp_to_string() == "_let x = 5 == 3\n_in  x");

    CHECK((NEW(EqExpr)(NEW(LetExpr)("x",NEW(NumExpr)(5),NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))),NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)))))->interp(Env::empty)->equals(NEW(BoolVal)(true)));


    CHECK(let1->equals(let2) == true);
    CHECK(let1->equals(nullLet) == false);
    CHECK(let2->interp(Env::empty)->equals(NEW(NumVal)(7)));

    CHECK_THROWS_WITH((NEW(LetExpr)("x", NEW(VarExpr)("t"), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))->interp(Env::empty), "free variable: t");
    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))->interp(Env::empty)->equals(NEW(NumVal)(6)));
    CHECK((NEW(LetExpr)("x", NEW(AddExpr)( NEW(NumExpr)(5), NEW(NumExpr) (2)), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))->interp(Env::empty)->equals(NEW(NumVal)(8)));

    CHECK(num1->equals(nullNum) == false);
    CHECK((NEW(MultExpr)(num2, num1))->equals(NEW(AddExpr)(num1, num2)) == false);
    CHECK((NEW(AddExpr)(num2, num1))->equals(NEW(MultExpr)(num1, num2)) == false);
    CHECK((myVar)->equals(NEW(MultExpr)(num1, num2)) == false);
    CHECK((num1)->interp(Env::empty)->equals(NEW(NumVal)(1)));
    CHECK((NEW(AddExpr)(num2, num1))->interp(Env::empty)->equals(NEW(NumVal)(3)));
    CHECK((NEW(MultExpr)(num2, num1))->interp(Env::empty)->equals(NEW(NumVal)(2)));
    std::string testString = "(_let x=5 _in ((_let y=3 _in (y+2))+x))";

    CHECK ((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))->to_string() == "(_let x=5 _in (x+1))");
    CHECK ((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(LetExpr)("y", NEW(NumExpr)(3), NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(2))), NEW(VarExpr)("x"))))->to_string() == "(_let x=5 _in ((_let y=3 _in (y+2))+x))");

    testString = "(_let x = 1\n _in  x + 2) + _let y = 3\n               _in  y + 4";
    CHECK((NEW(AddExpr)( NEW(LetExpr)( "x", NEW(NumExpr)(1), NEW(AddExpr)( NEW(VarExpr)("x"), NEW(NumExpr)(2))), NEW(LetExpr)( "y", NEW(NumExpr)(3), NEW(AddExpr)( NEW(VarExpr)("y"), NEW(NumExpr)(4)))))->pp_to_string() == testString);

    CHECK((NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->equals(NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4))) == true);
    CHECK((NEW(AddExpr)(num1, num2))->equals(NEW(AddExpr)(num1, num2)) == true);
    CHECK((NEW(AddExpr)(num3, num4))->equals(NEW(AddExpr)(num1, num2)) == false);
    CHECK((NEW(MultExpr)(num1, num2))->equals(NEW(MultExpr)(num1, num2)) == true);
    CHECK((NEW(MultExpr)(num3, num4))->equals(NEW(MultExpr)(num1, num2)) == false);
    CHECK((NEW(NumExpr)(5))->equals(NEW(NumExpr)(5)) == true);
    CHECK((NEW(NumExpr)(0))->equals(NEW(NumExpr)(5)) == false);
    CHECK((NEW(VarExpr)("hi"))->equals(NEW(VarExpr)("hi")) == true);
    CHECK((NEW(VarExpr)("ew"))->equals(NEW(VarExpr)("hi")) == false);
    CHECK((NEW(CallExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)))->equals(NULL) == false);
    CHECK_THROWS_WITH((NEW(VarExpr)("x"))->interp(Env::empty), "free variable: x");

    testString = "5";
    CHECK ((NEW(NumExpr)(5))->to_string() == "5");
    testString = "(5+4)";
    CHECK ((NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->to_string() == testString);
    testString = "(3+(5+4))";
    CHECK ((NEW(AddExpr)(NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4))))->to_string() == testString);
    testString = "(5*4)";
    CHECK ((NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->to_string() == testString);
    testString = "((3*2)+(5*6))";
    CHECK ((NEW(AddExpr)( NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->to_string() == testString);
    CHECK((NEW(FunExpr)("x", NEW(NumExpr)(5)))->to_string() == "(_fun (x) 5)");
    CHECK((NEW(CallExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)))->to_string() == "x(5)");

    testString = "5";
    CHECK ((NEW(NumExpr)(5))->pp_to_string() == "5");
    testString = "5 + 4";
    CHECK ((NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->pp_to_string() == testString);
    testString = "3 + 5 + 4";
    CHECK ((NEW(AddExpr)(NEW(NumExpr)(3), NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4))))->pp_to_string() == testString);
    testString = "5 * 4";
    CHECK ((NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->pp_to_string() == testString);
    testString = "3 * 2 + 5 * 6";
    CHECK ((NEW(AddExpr)( NEW(MultExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    testString = "(3 + 2) + 5 + 6";
    CHECK ((NEW(AddExpr)( NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(2)), NEW(AddExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);

    //base cases with Nums
    testString = "5";
    CHECK ((NEW(NumExpr)(5))->pp_to_string() == "5");
    testString = "5 + 4";
    CHECK ((NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->pp_to_string() == testString);
    testString = "5 * 4";
    CHECK ((NEW(MultExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->pp_to_string() == testString);
    //base cases with Nums
    testString = "x";
    CHECK ((NEW(VarExpr)("x"))->pp_to_string() == "x");
    testString = "x + 4";
    CHECK ((NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(4)))->pp_to_string() == testString);
    testString = "5 * x";
    CHECK ((NEW(MultExpr)(NEW(NumExpr)(5), NEW(VarExpr)("x")))->pp_to_string() == testString);
    //Add test cases
    testString = "(3 + 4) + 5";
    CHECK ((NEW(AddExpr)(NEW(AddExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(NumExpr)(5)))->pp_to_string() == testString);
    testString = "3 + 4 + 5";
    CHECK ((NEW(AddExpr)(NEW(NumExpr)(3), NEW(AddExpr)( NEW(NumExpr)(4), NEW(NumExpr)(5))))->pp_to_string() == testString);
    testString = "3 + 4 * 5";
    CHECK ((NEW(AddExpr)(NEW(NumExpr)(3), NEW(MultExpr)( NEW(NumExpr)(4), NEW(NumExpr)(5))))->pp_to_string() == testString);
    testString = "3 * 4 + 5";
    CHECK ((NEW(AddExpr)(NEW(MultExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(NumExpr)(5)))->pp_to_string() == testString);
    testString = "(3 + 4) + 5 + 6";
    CHECK ((NEW(AddExpr)(NEW(AddExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(AddExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    testString = "3 * 4 + 5 * 6";
    CHECK ((NEW(AddExpr)(NEW(MultExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    testString = "(3 + 4) + 5 * 6";
    CHECK ((NEW(AddExpr)(NEW(AddExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    testString = "3 * 4 + 5 + 6";
    CHECK ((NEW(AddExpr)(NEW(MultExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(AddExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    //Mult test cases
    testString = "(3 + 4) * 5";
    CHECK ((NEW(MultExpr)(NEW(AddExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(NumExpr)(5)))->pp_to_string() == testString);
    testString = "3 * (4 + 5)";
    CHECK ((NEW(MultExpr)(NEW(NumExpr)(3), NEW(AddExpr)( NEW(NumExpr)(4), NEW(NumExpr)(5))))->pp_to_string() == testString);
    testString = "3 * 4 * 5";
    CHECK ((NEW(MultExpr)(NEW(NumExpr)(3), NEW(MultExpr)( NEW(NumExpr)(4), NEW(NumExpr)(5))))->pp_to_string() == testString);
    testString = "(3 * 4) * 5";
    CHECK ((NEW(MultExpr)(NEW(MultExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(NumExpr)(5)))->pp_to_string() == testString);
    testString = "(3 + 4) * (5 + 6)";
    CHECK ((NEW(MultExpr)(NEW(AddExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(AddExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    testString = "(3 * 4) * 5 * 6";
    CHECK ((NEW(MultExpr)(NEW(MultExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    testString = "(3 + 4) * 5 * 6";
    CHECK ((NEW(MultExpr)(NEW(AddExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);
    testString = "(3 * 4) * (5 + 6)";
    CHECK ((NEW(MultExpr)(NEW(MultExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(AddExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))))->pp_to_string() == testString);

    testString = "((3 * 4) * 5 * 6) * (7 + 8)";
    CHECK ((NEW(MultExpr)(NEW(MultExpr)( NEW(MultExpr)( NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(NumExpr)(6))), NEW(AddExpr)( NEW(NumExpr)(7), NEW(NumExpr)(8))))->pp_to_string() == testString);

    testString = "((c + d) * 6) * 5 * f";
    CHECK ((NEW(MultExpr)(NEW(MultExpr)( NEW(AddExpr)( NEW(VarExpr)("c"), NEW(VarExpr)("d")), NEW(NumExpr)(6)), NEW(MultExpr)( NEW(NumExpr)(5), NEW(VarExpr)("f"))))->pp_to_string() == testString);

    CHECK_THROWS_WITH((NEW(IfExpr)(NEW(BoolExpr)(false), NEW(NumExpr)(5), NEW(VarExpr)("x")))->interp(Env::empty), "free variable: x");

    testString = "_true";
    CHECK ((NEW(BoolExpr)(true))->to_string() == testString);
    CHECK ((NEW(BoolExpr)(true))->pp_to_string() == testString);
    testString = "_false";
    CHECK ((NEW(BoolExpr)(false))->to_string() == testString);

    BoolExpr nullBool = NULL;

    CHECK(nullBool.equals(nullLet) == false);

    CHECK((NEW(BoolVal)(true))->is_true() == true);
    CHECK((NEW(BoolVal)(false))->is_true() == false);

    CHECK((NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5)))->interp(Env::empty)->equals(NEW(BoolVal)(true)));
    CHECK((NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(5)))->interp(Env::empty)->equals(NEW(BoolVal)(false)));
    CHECK((NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5)))->interp(Env::empty)->equals(NEW(BoolVal)(true)));

    CHECK((NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(1), NEW(NumExpr)(2)))->interp(Env::empty)->equals(NEW(NumVal)(1)));
    CHECK((NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5)))->equals(NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5))));
    CHECK((NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5)))->equals(nullNum) == false);

    CHECK ((NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5)))->to_string() == "(5==5)");
    CHECK ((NEW(EqExpr)(NEW(NumExpr)(5), NEW(NumExpr)(5)))->pp_to_string() == "5 == 5");

    CHECK((NEW(IfExpr)(NEW(BoolExpr)(false), NEW(NumExpr)(5), NEW(NumExpr)(2)))->to_string() == "(_if _false _then 5 _else 2)");
    CHECK((NEW(IfExpr)(NEW(BoolExpr)(false), NEW(NumExpr)(5), NEW(NumExpr)(2)))->equals(nullNum) == false);
    
    CHECK ((NEW(BoolExpr)(true))->pp_to_string() == "_true");
    CHECK ((NEW(BoolExpr)(false))->pp_to_string() == "_false");
    
    CHECK ((NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(0)))->to_string() == "(0==0)");
    CHECK ((NEW(EqExpr)(NEW(BoolExpr)(true), NEW(BoolExpr)(false)))->to_string() == "(_true==_false)");
    
    CHECK ((NEW(IfExpr)(NEW(EqExpr)(NEW(NumExpr)(0), NEW(NumExpr)(0)), NEW(NumExpr)(2), NEW(NumExpr)(1)))->to_string() == "(_if (0==0) _then 2 _else 1)");
    
    CHECK((NEW(AddExpr) (NEW(EqExpr)(NEW(NumExpr)(1), NEW(NumExpr)(0)), NEW(NumExpr)(1)))->pp_to_string() == "(1 == 0) + 1");
    
    testString = "(_let x = 2\n _in  x) == _let x = 4\n            _in  x";
    CHECK((NEW(EqExpr) (NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(VarExpr)("x")), NEW(LetExpr)("x",  NEW(NumExpr)(4), NEW(VarExpr)("x"))))->pp_to_string() == testString);
    
    testString = "_let x = 3\n_in  x == 3";
    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(EqExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->pp_to_string() == testString);
    
    testString = "(_let x = 3\n _in  x) == 3";
    CHECK((NEW(EqExpr)(NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(VarExpr)("x")), NEW(NumExpr)(3)))->pp_to_string() == testString);
    
    testString = "_if 1 + 3\n_then 4\n_else 6";
    CHECK((NEW(IfExpr)(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(3)), NEW(NumExpr)(4), NEW(NumExpr)(6)))->pp_to_string() == testString);
    
    testString = "_if _if 3 == 4\n    _then _false\n    _else _true\n_then 5\n_else 6";
    CHECK((NEW(IfExpr)(NEW(IfExpr)(NEW(EqExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(BoolExpr)(false), NEW(BoolExpr)(true)), NEW(NumExpr)(5), NEW(NumExpr)(6)))->pp_to_string() == testString);
    
    CHECK((NEW(EqExpr)(NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(2), NEW(NumExpr)(-2)), NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(5), NEW(NumExpr)(-5))))->pp_to_string()== "(_if _true\n _then 2\n _else -2) == _if _true\n              _then 5\n              _else -5");
    
    
    CHECK((NEW(LetExpr)("factrl", NEW(FunExpr)("factrl", NEW(FunExpr)("x", NEW(IfExpr)(NEW(EqExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)), NEW(NumExpr)(1), NEW(MultExpr)(NEW(VarExpr)("x"), NEW(CallExpr)( NEW(CallExpr)( NEW(VarExpr)("factrl"), NEW(VarExpr)("factrl")),NEW(AddExpr)( NEW(VarExpr)("x"), NEW(NumExpr)(-1))))))), NEW(CallExpr)( NEW(CallExpr)( NEW(VarExpr)("factrl"), NEW(VarExpr)("factrl")), NEW(NumExpr)(10))))->interp(Env::empty)->equals(NEW(NumVal)(3628800)));

    CHECK(Step::interp_by_steps(parse_str("1"))->equals(NEW(NumVal)(1)));
    CHECK(Step::interp_by_steps(parse_str("2+2"))->equals(NEW(NumVal)(4)));
    CHECK(Step::interp_by_steps(parse_str("_true"))->equals(NEW(BoolVal)(true)));
    CHECK(Step::interp_by_steps(parse_str("_let x = 3 _in x+2"))->equals(NEW(NumVal)(5)));
    CHECK(Step::interp_by_steps(parse_str("_if 1==1 _then 5 _else 3"))->equals(NEW(NumVal)(5)));
    CHECK(Step::interp_by_steps(parse_str("_if 1==0 _then 5 _else 3"))->equals(NEW(NumVal)(3)));
    CHECK(Step::interp_by_steps(parse_str("_if 1==1 _then 2*2 _else 3"))->equals(NEW(NumVal)(4)));
    CHECK(Step::interp_by_steps(parse_str("_let f = _fun (x) x + 1 _in  f(10)"))->equals(NEW(NumVal)(11)));
    CHECK(Step::interp_by_steps(parse_str("(_fun (x) x + 1)(5)"))->equals(NEW(NumVal)(6)));

};
