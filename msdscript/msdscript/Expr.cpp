//
//  Expr.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/24/21.
//

#include "Expr.h"
#include "catch.h"
#include "Parse.h"
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
    this->val = val;
}

bool NumExpr::equals(Expr *other){
    NumExpr *num = dynamic_cast<NumExpr*>(other);
    if(num == NULL)
        return false;
    else
        return this->val == num->val;
}

Val* NumExpr::interp(){
    return new NumVal(this->val);
}

bool NumExpr::has_variable(){
    return false;
}

Expr* NumExpr::subst(std::string string,Expr *exp){
    return this;
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


AddExpr::AddExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(Expr *other){
    AddExpr *o = dynamic_cast<AddExpr*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
};

Val* AddExpr::interp(){
    return this->lhs->interp()->add_to(this->rhs->interp());
};

bool AddExpr::has_variable(){
    return (this->lhs->has_variable() || this->rhs->has_variable());
};

Expr* AddExpr::subst(std::string string, Expr *exp){
    return new AddExpr(lhs->subst(string, exp),rhs->subst(string, exp));
};

void AddExpr::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "+";
    this->rhs->print(output);
    output << ")";
};

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
};


MultExpr::MultExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
};

bool MultExpr::equals(Expr *other){
    MultExpr *o = dynamic_cast<MultExpr*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
};

Val* MultExpr::interp(){
    return this->lhs->interp()->mult_to(this->rhs->interp());
};

bool MultExpr::has_variable(){
    return (this->lhs->has_variable() || this->rhs->has_variable());
};

Expr* MultExpr::subst(std::string string, Expr *exp){
    return new MultExpr(lhs->subst(string, exp), rhs->subst(string, exp));
}

void MultExpr::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "*";
    this->rhs->print(output);
    output << ")";
};

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
};

VarExpr::VarExpr(std::string var){
    this->var = var;
};

bool VarExpr::equals(Expr *other){
    VarExpr *o = dynamic_cast<VarExpr*>(other);
    if(o == NULL)
        return false;
    else
        return(this->var == o->var);
};

Val* VarExpr::interp(){
    throw std::runtime_error("variable has no value");
};

bool VarExpr::has_variable(){
    return true;
};

Expr* VarExpr::subst(std::string string, Expr *exp){
    if(this->var == string)
        return exp;
    else
        return this;
};

void VarExpr::print(std::ostream& output){
    output << this->var;
};

void VarExpr::pretty_print(std::ostream& output){
    output << this->var;
};

void VarExpr::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    output << this->var;
}

LetExpr::LetExpr(std::string lhs, Expr *rhs, Expr *body){
    this->lhs = lhs;
    this->rhs = rhs;
    this->body = body;
}

bool LetExpr::equals(Expr *other){
    LetExpr *o = dynamic_cast<LetExpr*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs == o->lhs && this->rhs->equals(o->rhs) && this->body->equals(o->body));
}

Val* LetExpr::interp(){
    Val* n = this->rhs->interp();
    return this->body->subst(this->lhs, n->to_expr())->interp();
}

bool LetExpr::has_variable(){
    return (this->rhs->has_variable() || this->body->has_variable());
}

Expr* LetExpr::subst(std::string string, Expr* exp){
    std::string new_lhs = this->lhs;
    Expr *new_rhs = this->rhs->subst(string, exp);
    Expr *new_body = this->body;
    if(new_lhs != string){
        new_body = new_body->subst(string, exp);
    }
    return new LetExpr(new_lhs, new_rhs, new_body);
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
    this->boolVal = boolVal;
}

bool BoolExpr::equals(Expr *other){
    BoolExpr *b = dynamic_cast<BoolExpr*>(other);
    if(b == NULL)
        return false;
    else
        return this->boolVal == b->boolVal;
}

Val* BoolExpr::interp(){
    return new BoolVal(this->boolVal);
}

bool BoolExpr::has_variable(){
    return false;
}

Expr* BoolExpr::subst(std::string string,Expr *exp){
    return this;
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

EqExpr::EqExpr(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
};

bool EqExpr::equals(Expr *other){
    EqExpr *e = dynamic_cast<EqExpr*>(other);
    if(e == NULL)
        return false;
    else
        return (this->lhs->equals(e->lhs) && this->rhs->equals(e->rhs));
};

Val* EqExpr::interp(){
    return new BoolVal(this->lhs->interp()->equals(this->rhs->interp()));
};

bool EqExpr::has_variable(){
    return (this->lhs->has_variable() || this->rhs->has_variable());
};

Expr* EqExpr::subst(std::string string, Expr *exp){
    return new EqExpr(lhs->subst(string, exp), rhs->subst(string, exp));
}

void EqExpr::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "==";
    this->rhs->print(output);
    output << ")";
};

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
};

IfExpr::IfExpr(Expr *test_part, Expr *then_part, Expr *else_part){
    this->test_part = test_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

bool IfExpr::equals(Expr *other){
    IfExpr *o = dynamic_cast<IfExpr*>(other);
    if(o == NULL)
        return false;
    else
        return (this->test_part->equals(o->test_part) && this->then_part->equals(o->then_part) && this->else_part->equals(o->else_part));
}

Val* IfExpr::interp(){
    if(test_part->interp()->is_true())
        return then_part->interp();
    else
        return else_part->interp();
}

bool IfExpr::has_variable(){
    return (this->test_part->has_variable() || this->then_part->has_variable() || this->else_part->has_variable());
}

Expr* IfExpr::subst(std::string string, Expr* exp){
    return new IfExpr(test_part->subst(string, exp), then_part->subst(string, exp), else_part->subst(string, exp));
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

TEST_CASE("Expression Tests"){
    std::ostream& os = std::cout;
    std::stringstream ss;
    NumExpr *num1 = new NumExpr(1);
    NumExpr *num2 = new NumExpr(2);
    NumExpr *num3 = new NumExpr(3);
    NumExpr *num4 = new NumExpr(4);
    VarExpr *myVar = new VarExpr("x");
    VarExpr *myVar2 = new VarExpr("y");
    NumExpr *nullNum = NULL;
    nullNum = NULL;
    LetExpr *nullLet = NULL;

    LetExpr *let1 = new LetExpr("x", new NumExpr(2), new AddExpr(new NumExpr(5), new VarExpr("x")));
    LetExpr *let2 = new LetExpr("x", new NumExpr(2), new AddExpr(new NumExpr(5), new VarExpr("x")));
    std::string pp_let1 = "_let x = 5\n        _in  (_let y = 3\n              _in  y + 2) + x";

    Expr *let3 = new LetExpr("x", new NumExpr(1), new AddExpr(new VarExpr("y"), new NumExpr(2)));
    CHECK((let3)->subst("x", new AddExpr(new VarExpr("y"), new NumExpr(3)))->equals(let3));

    Expr *let4 = new LetExpr("x",new VarExpr("x"),new AddExpr(new VarExpr("x"), new NumExpr(2)));
    CHECK((let4->subst("x", new NumExpr(5))->equals(new LetExpr("x",new NumExpr(5),new AddExpr (new VarExpr("x"), new NumExpr(2))))));

    Expr *let5 = new LetExpr ("x", new NumExpr(8), new AddExpr (new VarExpr("x"), new VarExpr("y")));
    CHECK((let5-> subst("y", new NumExpr(9)))->equals(new LetExpr ("x", new NumExpr(8),new AddExpr (new VarExpr("x"), new NumExpr(9)))));

    Expr *let6 = new LetExpr ("x",new VarExpr ("y"),new AddExpr (new VarExpr("x"), new VarExpr("y")));
    CHECK((let6-> subst("y", new NumExpr(9)))->equals(new LetExpr ("x", new NumExpr(9),new AddExpr (new VarExpr("x"), new NumExpr(9)))));
    CHECK((new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new VarExpr("x"))), new NumExpr (1)))->interp()->equals(new NumVal(26)));
    CHECK((new AddExpr(new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new VarExpr("x"))), new NumExpr (1)))->pp_to_string() == "5 * (_let x = 5\n     _in  x) + 1");

    CHECK((new MultExpr(new NumExpr(5), new AddExpr(new LetExpr("x", new NumExpr(5), new VarExpr("x")), new NumExpr (1))))->interp()->equals(new NumVal(30)));
    CHECK((new MultExpr(new NumExpr(5), new AddExpr(new LetExpr("x", new NumExpr(5), new VarExpr("x")), new NumExpr (1))))->pp_to_string() == "5 * ((_let x = 5\n      _in  x) + 1)");

    CHECK((new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr (1)))))->pp_to_string() == "5 * _let x = 5\n    _in  x + 1");
    CHECK((new MultExpr(new NumExpr(5), new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr (1)))))->interp()->equals(new NumVal(30)));

    CHECK ((new MultExpr( new NumExpr (5), (new LetExpr("x", new NumExpr(5), new LetExpr("y", new NumExpr(3), new LetExpr( "z", new NumExpr(1), new AddExpr (new VarExpr("z"), new NumExpr(4))))))))->pp_to_string() == "5 * _let x = 5\n    _in  _let y = 3\n         _in  _let z = 1\n              _in  z + 4");
    //Let pretty print
    CHECK ((new LetExpr("x", new NumExpr(5), new LetExpr("x", new NumExpr(1), new AddExpr( new VarExpr("x"), new NumExpr(3)))))->pp_to_string() == "_let x = 5\n_in  _let x = 1\n     _in  x + 3");

    pp_let1 = "_let x = 5\n_in  x + 1";
    CHECK ((new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1))))->pp_to_string() == pp_let1);

    pp_let1 = "_let x = 5\n_in  _let x = 2\n     _in  x + 3";
    CHECK ((new LetExpr("x", new NumExpr(5), new LetExpr("x", new NumExpr(2), new AddExpr( new VarExpr("x"), new NumExpr(3)))))->pp_to_string()== pp_let1);

    pp_let1 = "_let x = 5\n_in  (_let y = 3\n      _in  y + 2) + x";
    CHECK ((new LetExpr("x", new NumExpr(5), new AddExpr(new LetExpr("y", new NumExpr(3), new AddExpr(new VarExpr("y"), new NumExpr(2))), new VarExpr("x"))))
           ->pp_to_string() == pp_let1);


    CHECK((new EqExpr(new NumExpr(0), new NumExpr(3)))->pp_to_string() == "0 == 3");

    CHECK((new AddExpr (new EqExpr(new NumExpr(0), new NumExpr(3)), new NumExpr(6)))->pp_to_string() == "(0 == 3) + 6");
    CHECK((new MultExpr (new EqExpr(new NumExpr(0), new NumExpr(3)), new NumExpr(6)))->pp_to_string() == "(0 == 3) * 6");
    CHECK((new AddExpr (new NumExpr(6), new EqExpr(new NumExpr(0), new NumExpr(3))))->pp_to_string() == "6 + (0 == 3)");
    CHECK((new MultExpr (new EqExpr(new NumExpr(7), new NumExpr(4)), new NumExpr(6)))->pp_to_string() == "(7 == 4) * 6");
    CHECK((new MultExpr (new NumExpr(6), new EqExpr(new NumExpr(0), new NumExpr(3))))->pp_to_string() == "6 * (0 == 3)");
    CHECK((new EqExpr(new EqExpr(new NumExpr(1), new NumExpr(2)), new EqExpr(new NumExpr(3), new NumExpr(4))))->pp_to_string() == "(1 == 2) == 3 == 4");

    CHECK((new EqExpr(new LetExpr("x", new NumExpr(5), new VarExpr("x")), new LetExpr("x", new NumExpr(5), new VarExpr("x"))))->pp_to_string() == "(_let x = 5\n _in  x) == _let x = 5\n            _in  x");

    CHECK((new EqExpr(new LetExpr("x", new NumExpr(5), new VarExpr("x")), new LetExpr("x", new NumExpr(5), new VarExpr("x"))))->interp()->equals(new BoolVal(true)));

    CHECK((new EqExpr(new LetExpr("x", new NumExpr(5),new AddExpr(new VarExpr("x"), new NumExpr(3))),new LetExpr("x",new NumExpr(3),new AddExpr(new VarExpr("x"), new NumExpr(5)))))->pp_to_string() == "(_let x = 5\n _in  x + 3) == _let x = 3\n                _in  x + 5");

    CHECK((new LetExpr("x", new EqExpr(new NumExpr(5), new NumExpr(3)), new VarExpr("x")))->pp_to_string() == "_let x = 5 == 3\n_in  x");

    CHECK((new EqExpr(new LetExpr("x",new NumExpr(5),new AddExpr(new VarExpr("x"), new NumExpr(3))),new LetExpr("x", new NumExpr(3), new AddExpr(new VarExpr("x"), new NumExpr(5)))))->interp()->equals(new BoolVal(true)));


    CHECK(let1->equals(let2) == true);
    CHECK(let1->equals(nullLet) == false);
    CHECK(let1->has_variable() == true);
    CHECK(let2->interp()->equals(new NumVal(7)));
    CHECK(let2->subst("x", num3)->interp()->equals(new NumVal(7)));

    CHECK_THROWS_WITH((new LetExpr("x", new VarExpr("t"), new AddExpr(new VarExpr("x"), new NumExpr(1))))->interp(), "variable has no value");
    CHECK((new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1))))->interp()->equals(new NumVal(6)));
    CHECK((new LetExpr("x", new AddExpr( new NumExpr(5), new NumExpr (2)), new AddExpr(new VarExpr("x"), new NumExpr(1))))->interp()->equals(new NumVal(8)));

    CHECK(num1->equals(nullNum) == false);
    CHECK((new MultExpr(num2, num1))->equals(new AddExpr(num1, num2)) == false);
    CHECK((new AddExpr(num2, num1))->equals(new MultExpr(num1, num2)) == false);
    CHECK((myVar)->equals(new MultExpr(num1, num2)) == false);
    CHECK((num1)->interp()->equals(new NumVal(1)));
    CHECK((new AddExpr(myVar, num1))->has_variable() == true);
    CHECK((new AddExpr(num1, myVar))->has_variable() == true);
    CHECK((new AddExpr(num2, num1))->interp()->equals(new NumVal(3)));
    CHECK((new MultExpr(num2, num1))->interp()->equals(new NumVal(2)));
    CHECK((new MultExpr(num1, myVar))->has_variable() == true);
    CHECK((new MultExpr(num1, num2))->has_variable() == false);
    myVar2->print(os);
    std::cout << "=y\n";
    std::string testString = "(_let x=5 _in ((_let y=3 _in (y+2))+x))";

    CHECK ((new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1))))->to_string() == "(_let x=5 _in (x+1))");
    CHECK ((new LetExpr("x", new NumExpr(5), new AddExpr(new LetExpr("y", new NumExpr(3), new AddExpr(new VarExpr("y"), new NumExpr(2))), new VarExpr("x"))))->to_string() == "(_let x=5 _in ((_let y=3 _in (y+2))+x))");

    testString = "(_let x = 1\n _in  x + 2) + _let y = 3\n               _in  y + 4";
    CHECK((new AddExpr( new LetExpr( "x", new NumExpr(1), new AddExpr( new VarExpr("x"), new NumExpr(2))), new LetExpr( "y", new NumExpr(3), new AddExpr( new VarExpr("y"), new NumExpr(4)))))->pp_to_string() == testString);

    CHECK((new AddExpr(new NumExpr(5), new NumExpr(4)))->equals(new AddExpr(new NumExpr(5), new NumExpr(4))) == true);
    CHECK((new AddExpr(num1, num2))->equals(new AddExpr(num1, num2)) == true);
    CHECK((new AddExpr(num3, num4))->equals(new AddExpr(num1, num2)) == false);
    CHECK((new MultExpr(num1, num2))->equals(new MultExpr(num1, num2)) == true);
    CHECK((new MultExpr(num3, num4))->equals(new MultExpr(num1, num2)) == false);
    CHECK((new NumExpr(5))->equals(new NumExpr(5)) == true);
    CHECK((new NumExpr(0))->equals(new NumExpr(5)) == false);
    CHECK((new VarExpr("hi"))->equals(new VarExpr("hi")) == true);
    CHECK((new VarExpr("ew"))->equals(new VarExpr("hi")) == false);
    CHECK_THROWS_WITH((new VarExpr("x"))->interp(), "variable has no value");
    CHECK( (new AddExpr(new VarExpr("x"), new NumExpr(7)))->subst("x", new VarExpr("y"))->equals(new AddExpr(new VarExpr("y"), new NumExpr(7))) );


    CHECK((new NumExpr(8))->subst("x", new NumExpr(8))->equals(new NumExpr(8)));
    CHECK( (new MultExpr(new VarExpr("x"), new NumExpr(7)))->subst("x", new VarExpr("y"))->equals(new MultExpr(new VarExpr("y"), new NumExpr(7))) );


    CHECK( (new VarExpr("x"))->subst("y", new VarExpr("y"))->equals(new VarExpr("x")) == true);

    testString = "5";
    CHECK ((new NumExpr(5))->to_string() == "5");
    testString = "(5+4)";
    CHECK ((new AddExpr(new NumExpr(5), new NumExpr(4)))->to_string() == testString);
    testString = "(3+(5+4))";
    CHECK ((new AddExpr(new NumExpr(3), new AddExpr(new NumExpr(5), new NumExpr(4))))->to_string() == testString);
    testString = "(5*4)";
    CHECK ((new MultExpr(new NumExpr(5), new NumExpr(4)))->to_string() == testString);
    testString = "((3*2)+(5*6))";
    CHECK ((new AddExpr( new MultExpr(new NumExpr(3), new NumExpr(2)), new MultExpr( new NumExpr(5), new NumExpr(6))))->to_string() == testString);

    testString = "5";
    CHECK ((new NumExpr(5))->pp_to_string() == "5");
    testString = "5 + 4";
    CHECK ((new AddExpr(new NumExpr(5), new NumExpr(4)))->pp_to_string() == testString);
    testString = "3 + 5 + 4";
    CHECK ((new AddExpr(new NumExpr(3), new AddExpr(new NumExpr(5), new NumExpr(4))))->pp_to_string() == testString);
    testString = "5 * 4";
    CHECK ((new MultExpr(new NumExpr(5), new NumExpr(4)))->pp_to_string() == testString);
    testString = "3 * 2 + 5 * 6";
    CHECK ((new AddExpr( new MultExpr(new NumExpr(3), new NumExpr(2)), new MultExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    testString = "(3 + 2) + 5 + 6";
    CHECK ((new AddExpr( new AddExpr(new NumExpr(3), new NumExpr(2)), new AddExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);

    //base cases with Nums
    testString = "5";
    CHECK ((new NumExpr(5))->pp_to_string() == "5");
    testString = "5 + 4";
    CHECK ((new AddExpr(new NumExpr(5), new NumExpr(4)))->pp_to_string() == testString);
    testString = "5 * 4";
    CHECK ((new MultExpr(new NumExpr(5), new NumExpr(4)))->pp_to_string() == testString);
    //base cases with Nums
    testString = "x";
    CHECK ((new VarExpr("x"))->pp_to_string() == "x");
    testString = "x + 4";
    CHECK ((new AddExpr(new VarExpr("x"), new NumExpr(4)))->pp_to_string() == testString);
    testString = "5 * x";
    CHECK ((new MultExpr(new NumExpr(5), new VarExpr("x")))->pp_to_string() == testString);
    //Add test cases
    testString = "(3 + 4) + 5";
    CHECK ((new AddExpr(new AddExpr( new NumExpr(3), new NumExpr(4)), new NumExpr(5)))->pp_to_string() == testString);
    testString = "3 + 4 + 5";
    CHECK ((new AddExpr(new NumExpr(3), new AddExpr( new NumExpr(4), new NumExpr(5))))->pp_to_string() == testString);
    testString = "3 + 4 * 5";
    CHECK ((new AddExpr(new NumExpr(3), new MultExpr( new NumExpr(4), new NumExpr(5))))->pp_to_string() == testString);
    testString = "3 * 4 + 5";
    CHECK ((new AddExpr(new MultExpr( new NumExpr(3), new NumExpr(4)), new NumExpr(5)))->pp_to_string() == testString);
    testString = "(3 + 4) + 5 + 6";
    CHECK ((new AddExpr(new AddExpr( new NumExpr(3), new NumExpr(4)), new AddExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    testString = "3 * 4 + 5 * 6";
    CHECK ((new AddExpr(new MultExpr( new NumExpr(3), new NumExpr(4)), new MultExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    testString = "(3 + 4) + 5 * 6";
    CHECK ((new AddExpr(new AddExpr( new NumExpr(3), new NumExpr(4)), new MultExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    testString = "3 * 4 + 5 + 6";
    CHECK ((new AddExpr(new MultExpr( new NumExpr(3), new NumExpr(4)), new AddExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    //Mult test cases
    testString = "(3 + 4) * 5";
    CHECK ((new MultExpr(new AddExpr( new NumExpr(3), new NumExpr(4)), new NumExpr(5)))->pp_to_string() == testString);
    testString = "3 * (4 + 5)";
    CHECK ((new MultExpr(new NumExpr(3), new AddExpr( new NumExpr(4), new NumExpr(5))))->pp_to_string() == testString);
    testString = "3 * 4 * 5";
    CHECK ((new MultExpr(new NumExpr(3), new MultExpr( new NumExpr(4), new NumExpr(5))))->pp_to_string() == testString);
    testString = "(3 * 4) * 5";
    CHECK ((new MultExpr(new MultExpr( new NumExpr(3), new NumExpr(4)), new NumExpr(5)))->pp_to_string() == testString);
    testString = "(3 + 4) * (5 + 6)";
    CHECK ((new MultExpr(new AddExpr( new NumExpr(3), new NumExpr(4)), new AddExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    testString = "(3 * 4) * 5 * 6";
    CHECK ((new MultExpr(new MultExpr( new NumExpr(3), new NumExpr(4)), new MultExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    testString = "(3 + 4) * 5 * 6";
    CHECK ((new MultExpr(new AddExpr( new NumExpr(3), new NumExpr(4)), new MultExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);
    testString = "(3 * 4) * (5 + 6)";
    CHECK ((new MultExpr(new MultExpr( new NumExpr(3), new NumExpr(4)), new AddExpr( new NumExpr(5), new NumExpr(6))))->pp_to_string() == testString);

    testString = "((3 * 4) * 5 * 6) * (7 + 8)";
    CHECK ((new MultExpr(new MultExpr( new MultExpr( new NumExpr(3), new NumExpr(4)), new MultExpr( new NumExpr(5), new NumExpr(6))), new AddExpr( new NumExpr(7), new NumExpr(8))))->pp_to_string() == testString);

    testString = "((c + d) * 6) * 5 * f";
    CHECK ((new MultExpr(new MultExpr( new AddExpr( new VarExpr("c"), new VarExpr("d")), new NumExpr(6)), new MultExpr( new NumExpr(5), new VarExpr("f"))))->pp_to_string() == testString);

    CHECK_THROWS_WITH((new IfExpr(new BoolExpr(false), new NumExpr(5), new VarExpr("x")))->interp(), "variable has no value");

    testString = "_true";
    CHECK ((new BoolExpr(true))->to_string() == testString);
    CHECK ((new BoolExpr(true))->pp_to_string() == testString);
    testString = "_false";
    CHECK ((new BoolExpr(false))->to_string() == testString);
    CHECK((new BoolExpr(true))->subst("y", new NumExpr(5))->equals(new BoolExpr(true)));

    BoolExpr nullBool = NULL;

    CHECK((new BoolExpr(new BoolVal(true)))->equals(new BoolExpr(new BoolVal(true))));
    CHECK((new BoolExpr(new BoolVal(true)))->has_variable() == false);
    CHECK(nullBool.equals(nullLet) == false);
    CHECK((new BoolExpr(nullBool))->equals(new BoolExpr(new BoolVal(false))) == false);

    CHECK((new BoolVal(true))->is_true() == true);
    CHECK((new BoolVal(false))->is_true() == false);

    CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->interp()->equals(new BoolVal(true)));
    CHECK((new EqExpr(new NumExpr(0), new NumExpr(5)))->interp()->equals(new BoolVal(false)));
    CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->interp()->equals(new BoolVal(true)));

    CHECK((new IfExpr(new BoolExpr(true), new NumExpr(1), new NumExpr(2)))->interp()->equals(new NumVal(1)));
    CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(new EqExpr(new NumExpr(5), new NumExpr(5))));
    CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->equals(nullNum) == false);
    CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->has_variable() == false);

    CHECK((new EqExpr(new NumExpr(5), new NumExpr(5)))->subst("y", new NumExpr(5))->equals(new EqExpr(new NumExpr(5), new NumExpr(5))));

    CHECK ((new EqExpr(new NumExpr(5), new NumExpr(5)))->to_string() == "(5==5)");
    CHECK ((new EqExpr(new NumExpr(5), new NumExpr(5)))->pp_to_string() == "5 == 5");

    CHECK((new IfExpr(new BoolExpr(false), new NumExpr(5), new NumExpr(2)))->to_string() == "(_if _false _then 5 _else 2)");
    CHECK((new IfExpr(new BoolExpr(false), new NumExpr(5), new NumExpr(2)))->equals(nullNum) == false);

    CHECK((new IfExpr(new BoolExpr(false), new NumExpr(5), new NumExpr(2)))->has_variable() == false);

    CHECK((new IfExpr(new BoolExpr(false), new NumExpr(5), new NumExpr(2)))->subst("y", new NumExpr(5))->equals(new IfExpr(new BoolExpr(false), new NumExpr(5), new NumExpr(2))));
    
    CHECK ((new BoolExpr(true))->pp_to_string() == "_true");
    CHECK ((new BoolExpr(false))->pp_to_string() == "_false");
    
    CHECK ((new EqExpr(new NumExpr(0), new NumExpr(0)))->to_string() == "(0==0)");
    CHECK ((new EqExpr(new BoolExpr(true), new BoolExpr(false)))->to_string() == "(_true==_false)");
    
    CHECK ((new IfExpr(new EqExpr(new NumExpr(0), new NumExpr(0)), new NumExpr(2), new NumExpr(1)))->to_string() == "(_if (0==0) _then 2 _else 1)");
    
    CHECK((new AddExpr (new EqExpr(new NumExpr(1), new NumExpr(0)), new NumExpr(1)))->pp_to_string() == "(1 == 0) + 1");
    
    testString = "(_let x = 2\n _in  x) == _let x = 4\n            _in  x";
    CHECK((new EqExpr (new LetExpr("x", new NumExpr(2), new VarExpr("x")), new LetExpr("x",  new NumExpr(4), new VarExpr("x"))))->pp_to_string() == testString);
    
    testString = "_let x = 3\n_in  x == 3";
    CHECK((new LetExpr("x", new NumExpr(3), new EqExpr(new VarExpr("x"), new NumExpr(3))))->pp_to_string() == testString);
    
    testString = "(_let x = 3\n _in  x) == 3";
    CHECK((new EqExpr(new LetExpr("x", new NumExpr(3), new VarExpr("x")), new NumExpr(3)))->pp_to_string() == testString);
    
    testString = "_if 1 + 3\n_then 4\n_else 6";
    CHECK((new IfExpr(new AddExpr(new NumExpr(1), new NumExpr(3)), new NumExpr(4), new NumExpr(6)))->pp_to_string() == testString);
    
    testString = "_if _if 3 == 4\n    _then _false\n    _else _true\n_then 5\n_else 6";
    CHECK((new IfExpr(new IfExpr(new EqExpr(new NumExpr(3), new NumExpr(4)), new BoolExpr(false), new BoolExpr(true)), new NumExpr(5), new NumExpr(6)))->pp_to_string() == testString);
    
    CHECK((new EqExpr(new IfExpr(new BoolExpr(true), new NumExpr(2), new NumExpr(-2)), new IfExpr(new BoolExpr(true), new NumExpr(5), new NumExpr(-5))))->pp_to_string()== "(_if _true\n _then 2\n _else -2) == _if _true\n              _then 5\n              _else -5");
};
