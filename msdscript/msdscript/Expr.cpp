//
//  Expr.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/24/21.
//

#include "Expr.h"
#include "catch.h"
#include <stdexcept>

std::string Expr::to_string(){
    std::ostream output(nullptr);
    std::stringbuf strBuf;
    output.rdbuf(&strBuf);
    print(output);
    return strBuf.str();
};

std::string Expr::pp_to_string(){
    std::ostream output(nullptr);
    std::stringbuf strBuf;
    output.rdbuf(&strBuf);
    pretty_print(output);
    return strBuf.str();
};

Num::Num(int val) {
    this->val = val;
};

bool Num::equals(Expr *other){
    Num *num = dynamic_cast<Num*>(other);
    if(num == NULL)
        return false;
    else
        return this->val == num->val;
};

int Num::interp(){
    return this->val;
};

bool Num::has_variable(){
    return false;
};

Expr* Num::subst(std::string string,Expr *exp){
    return this;
};

void Num::print(std::ostream& output){
    output << this->val;
}

void Num::pretty_print(std::ostream& output){
    this->pretty_print_at(output, print_group_none);
};

void Num::pretty_print_at(std::ostream& output, print_mode_t mode){
    output << this->val;
}


Add::Add(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
};

bool Add::equals(Expr *other){
    Add *o = dynamic_cast<Add*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
};

int Add::interp(){
    return this->lhs->interp() + this->rhs->interp();
};

bool Add::has_variable(){
    return (this->lhs->has_variable() || this->rhs->has_variable());
};

Expr* Add::subst(std::string string, Expr *exp){
    return new Add(lhs->subst(string, exp),rhs->subst(string, exp));
};

void Add::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "+";
    this->rhs->print(output);
    output << ")";
};

void Add::pretty_print(std::ostream& output){
    lhs->pretty_print_at(output, print_group_add);
    output << " + ";
    rhs->pretty_print_at(output, print_group_none);
}

void Add::pretty_print_at(std::ostream& output, print_mode_t mode){
    if(mode == print_group_add || mode == print_group_add_or_mult){
        output << "(";
    }
    
    lhs->pretty_print_at(output, print_group_add);

    output << " + ";
    
    rhs->pretty_print_at(output, print_group_none);
    
    if(mode == print_group_add || mode == print_group_add_or_mult){
        output << ")";
    }
    
};

    
Mult::Mult(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
};

bool Mult::equals(Expr *other){
    Mult *o = dynamic_cast<Mult*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs->equals(o->lhs) && this->rhs->equals(o->rhs));
};

int Mult::interp(){
    return this->lhs->interp() * this->rhs->interp();
};

bool Mult::has_variable(){
    return (this->lhs->has_variable() || this->rhs->has_variable());
};

Expr* Mult::subst(std::string string, Expr *exp){
    return new Mult(lhs->subst(string, exp), rhs->subst(string, exp));
}

void Mult::print(std::ostream& output){
    output << "(";
    this->lhs->print(output);
    output << "*";
    this->rhs->print(output);
    output << ")";
};

void Mult::pretty_print(std::ostream& output){
    lhs->pretty_print_at(output, print_group_add_or_mult);
    output << " * ";
    rhs->pretty_print_at(output, print_group_add);
}

void Mult::pretty_print_at(std::ostream& output, print_mode_t mode){
    if(mode == print_group_add_or_mult){
    output << "(";
    }
    lhs->pretty_print_at(output, print_group_add_or_mult);

    output << " * ";

    rhs->pretty_print_at(output, print_group_none);
    
    if(mode == print_group_add_or_mult){
        output << ")";
    }
};

Var::Var(std::string var){
    this->var = var;
};

bool Var::equals(Expr *other){
    Var *o = dynamic_cast<Var*>(other);
    if(o == NULL)
        return false;
    else
        return(this->var == o->var);
};

int Var::interp(){
    throw std::runtime_error("variable has no value");
};

bool Var::has_variable(){
    return true;
};

Expr* Var::subst(std::string string, Expr *exp){
    if(this->var == string)
        return exp;
    else
        return this;
};

void Var::print(std::ostream& output){
    output << this->var;
};

void Var::pretty_print(std::ostream& output){
    pretty_print_at(output, print_group_none);
};

void Var::pretty_print_at(std::ostream& output, print_mode_t mode){
    output << this->var;
}


TEST_CASE("equals"){
    std::ostream& os = std::cout;
    Num *num1 = new Num(1);
    Num *num2 = new Num(2);
    Num *num3 = new Num(3);
    Num *num4 = new Num(4);
    Var *myVar = new Var("x");
    Var *myVar2 = new Var("y");
    Num *nullNum = NULL;
    nullNum = NULL;
    
    CHECK(num1->equals(nullNum) == false);
    CHECK((new Mult(num2, num1))->equals(new Add(num1, num2)) == false);
    CHECK((new Add(num2, num1))->equals(new Mult(num1, num2)) == false);
    CHECK((myVar)->equals(new Mult(num1, num2)) == false);
    CHECK((num1)->interp() == 1);
    CHECK((new Add(myVar, num1))->has_variable() == true);
    CHECK((new Add(num1, myVar))->has_variable() == true);
    CHECK((new Add(num2, num1))->interp() == 3);
    CHECK((new Mult(num2, num1))->interp() == 2);
    CHECK((new Mult(num1, myVar))->has_variable() == true);
    CHECK((new Mult(num1, num2))->has_variable() == false);
    myVar2->print(os);

    Var *numX = new Var("X");
    Var *numY = new Var("Y");
    Add *add3_3 = new Add(num3,num3);
    Add *add3_2 = new Add(num3,num2);
    Mult *mult3_2 = new Mult(num3,num2);
    Add *add1_2_3 = new Add(new Num(1), new Add(new Num(2), new Num(3)));
    Add *add1_2then3 = new Add(new Add(new Num(1), new Num(2)), new Num(3));
    Add *mult1_2_3 = new Add(new Num(1), new Mult(new Num(2), new Num(3)));
    Mult *mult1_2then3 = new Mult(new Add(new Num(1), new Num(2)), new Num(3));
    Mult *mult1then2_3 = new Mult(new Num(1),new Add( new Num(2), new Num(3)));
    Mult *mult3_3thenM3_2 = new Mult(add3_3,mult3_2);
    Mult *mult3_2thenMadd3_2 = new Mult(mult3_2,add3_2);
    num3->pretty_print(os);
    num2->pretty_print(os);
    numY->pretty_print(os);
    numX->pretty_print(os);
    add3_3->pretty_print(os);
    mult3_2->print(os);
    mult3_2->print(os);
    printf("\n");
    add1_2_3->pretty_print(os);
    printf("\n");
    add1_2then3->pretty_print(os);
    printf("\n");
    mult1_2_3->pretty_print(os);
    printf("\n");
    mult1_2then3->pretty_print(os);
    printf("\n");
    mult1then2_3->pretty_print(os);
    printf("\n");
    mult3_3thenM3_2->pretty_print(os);
    printf("\n");
    mult3_2thenMadd3_2->pretty_print(os);

    
    CHECK((new Add(new Num(5), new Num(4)))->equals(new Add(new Num(5), new Num(4))) == true);
    CHECK((new Add(num1, num2))->equals(new Add(num1, num2)) == true);
    CHECK((new Add(num3, num4))->equals(new Add(num1, num2)) == false);
    CHECK((new Mult(num1, num2))->equals(new Mult(num1, num2)) == true);
    CHECK((new Mult(num3, num4))->equals(new Mult(num1, num2)) == false);
    CHECK((new Num(5))->equals(new Num(5)) == true);
    CHECK((new Num(0))->equals(new Num(5)) == false);
    CHECK((new Var("hi"))->equals(new Var("hi")) == true);
    CHECK((new Var("ew"))->equals(new Var("hi")) == false);
    CHECK_THROWS_WITH((new Var("x"))->interp(), "variable has no value");
    CHECK( (new Add(new Var("x"), new Num(7)))
           ->subst("x", new Var("y"))
           ->equals(new Add(new Var("y"), new Num(7))) );
    CHECK((new Num(8))->subst("x", new Num(8))->equals(new Num(8)));
    CHECK( (new Mult(new Var("x"), new Num(7)))
           ->subst("x", new Var("y"))
           ->equals(new Mult(new Var("y"), new Num(7))) );
    CHECK( (new Var("x"))
           ->subst("y", new Var("y"))->equals(new Var("x")) == true);
    
    
    
    std::string testString = "5";
      CHECK ((new Num(5))->to_string() == "5");
      testString = "(5+4)";
      CHECK ((new Add(new Num(5), new Num(4)))->to_string() == testString);
      testString = "(3+(5+4))";
      CHECK ((new Add(new Num(3), new Add(new Num(5), new Num(4))))->to_string() == testString);
      testString = "(5*4)";
      CHECK ((new Mult(new Num(5), new Num(4)))->to_string() == testString);
      testString = "((3*2)+(5*6))";
      CHECK ((new Add( new Mult(new Num(3), new Num(2)), new Mult( new Num(5), new Num(6))))->to_string() == testString);
    
    testString = "5";
    CHECK ((new Num(5))->pp_to_string() == "5");
    testString = "5 + 4";
    CHECK ((new Add(new Num(5), new Num(4)))->pp_to_string() == testString);
    testString = "3 + 5 + 4";
    CHECK ((new Add(new Num(3), new Add(new Num(5), new Num(4))))->pp_to_string() == testString);
    testString = "5 * 4";
    CHECK ((new Mult(new Num(5), new Num(4)))->pp_to_string() == testString);
    testString = "3 * 2 + 5 * 6";
    CHECK ((new Add( new Mult(new Num(3), new Num(2)), new Mult( new Num(5), new Num(6))))->pp_to_string() == testString);
    testString = "(3 + 2) + 5 + 6";
    CHECK ((new Add( new Add(new Num(3), new Num(2)), new Add( new Num(5), new Num(6))))->pp_to_string() == testString);
    
    //base cases with Nums
        testString = "5";
        CHECK ((new Num(5))->pp_to_string() == "5");
        testString = "5 + 4";
        CHECK ((new Add(new Num(5), new Num(4)))->pp_to_string() == testString);
        testString = "5 * 4";
        CHECK ((new Mult(new Num(5), new Num(4)))->pp_to_string() == testString);
        //base cases with Nums
        testString = "x";
        CHECK ((new Var("x"))->pp_to_string() == "x");
        testString = "x + 4";
        CHECK ((new Add(new Var("x"), new Num(4)))->pp_to_string() == testString);
        testString = "5 * x";
        CHECK ((new Mult(new Num(5), new Var("x")))->pp_to_string() == testString);
        //Add test cases
        testString = "(3 + 4) + 5";
        CHECK ((new Add(new Add( new Num(3), new Num(4)), new Num(5)))->pp_to_string() == testString);
        testString = "3 + 4 + 5";
        CHECK ((new Add(new Num(3), new Add( new Num(4), new Num(5))))->pp_to_string() == testString);
        testString = "3 + 4 * 5";
        CHECK ((new Add(new Num(3), new Mult( new Num(4), new Num(5))))->pp_to_string() == testString);
        testString = "3 * 4 + 5";
        CHECK ((new Add(new Mult( new Num(3), new Num(4)), new Num(5)))->pp_to_string() == testString);
        testString = "(3 + 4) + 5 + 6";
        CHECK ((new Add(new Add( new Num(3), new Num(4)), new Add( new Num(5), new Num(6))))->pp_to_string() == testString);
        testString = "3 * 4 + 5 * 6";
        CHECK ((new Add(new Mult( new Num(3), new Num(4)), new Mult( new Num(5), new Num(6))))->pp_to_string() == testString);
        testString = "(3 + 4) + 5 * 6";
        CHECK ((new Add(new Add( new Num(3), new Num(4)), new Mult( new Num(5), new Num(6))))->pp_to_string() == testString);
        testString = "3 * 4 + 5 + 6";
        CHECK ((new Add(new Mult( new Num(3), new Num(4)), new Add( new Num(5), new Num(6))))->pp_to_string() == testString);
        //Mult test cases
        testString = "(3 + 4) * 5";
        CHECK ((new Mult(new Add( new Num(3), new Num(4)), new Num(5)))->pp_to_string() == testString);
        testString = "3 * (4 + 5)";
        CHECK ((new Mult(new Num(3), new Add( new Num(4), new Num(5))))->pp_to_string() == testString);
        testString = "3 * 4 * 5";
        CHECK ((new Mult(new Num(3), new Mult( new Num(4), new Num(5))))->pp_to_string() == testString);
        testString = "(3 * 4) * 5";
        CHECK ((new Mult(new Mult( new Num(3), new Num(4)), new Num(5)))->pp_to_string() == testString);
        testString = "(3 + 4) * (5 + 6)";
        CHECK ((new Mult(new Add( new Num(3), new Num(4)), new Add( new Num(5), new Num(6))))->pp_to_string() == testString);
        testString = "(3 * 4) * 5 * 6";
        CHECK ((new Mult(new Mult( new Num(3), new Num(4)), new Mult( new Num(5), new Num(6))))->pp_to_string() == testString);
        testString = "(3 + 4) * 5 * 6";
        CHECK ((new Mult(new Add( new Num(3), new Num(4)), new Mult( new Num(5), new Num(6))))->pp_to_string() == testString);
        testString = "(3 * 4) * (5 + 6)";
        CHECK ((new Mult(new Mult( new Num(3), new Num(4)), new Add( new Num(5), new Num(6))))->pp_to_string() == testString);
    
    testString = "((3 * 4) * 5 * 6) * (7 + 8)";
    CHECK ((new Mult(new Mult( new Mult( new Num(3), new Num(4)), new Mult( new Num(5), new Num(6))), new Add( new Num(7), new Num(8))))
           ->pp_to_string() == testString);
    testString = "((a + b) * 7) * 6 * c";
    CHECK ((new Mult(new Mult( new Add( new Var("a"), new Var("b")), new Num(7)), new Mult( new Num(6), new Var("c"))))
           ->pp_to_string() == testString);
    
};
