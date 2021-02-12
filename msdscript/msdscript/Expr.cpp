//
//  Expr.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/24/21.
//

#include "Expr.h"
#include "catch.h"
#include <stdexcept>

static void consume(std::istream &in, int expect){
    int c = in.get();
    if(c != expect){
        char c_as_char = (char)c;
        char expect_as_char = (char) expect;
        std::cout << "your c value was " << c_as_char << " but we expected: " << expect_as_char;
        throw std::runtime_error("consume mismatch");
    }
}

static void skip_whitespace(std::istream &in){
    while(1){
        int c = in.peek();
        if(!isspace(c))
            break;
        consume(in, c);
    }
}

static void parse_keyword(std::istream &in, std::string keyword){
    std::string word = "";
    for(int i = 0; i < keyword.length(); i++){
        word += in.get();
    }
    if(word != keyword)
        throw std::runtime_error("keyword not found");
    return;
}

Expr* Expr::parse_var(std::istream &in){
    std::string var = "";
    skip_whitespace(in);
    int c = in.peek();
    while(isalpha(c)){
        c = in.get();
        var += c;
        c = in.peek();
    }
    return new Var(var);
}

Expr* Expr::parse_let(std::istream &in){
    skip_whitespace(in);
    consume(in, '_');
    parse_keyword(in, "let");
    skip_whitespace(in);
    Expr *v = parse_var(in);
    skip_whitespace(in);
    consume(in, '=');
    skip_whitespace(in);
    Expr *rhs = parse_expr(in);
    skip_whitespace(in);
    consume(in, '_');
    parse_keyword(in, "in");
    skip_whitespace(in);
    Expr *body = parse_expr(in);
    return new Let(v->to_string(), rhs, body);
}

Expr* Expr::parse(std::istream &in){
    Expr *e = parse_expr(in);
    return e;
}

Expr* Expr::parse_str(std::string s){
    std::istringstream stream1(s);
    Expr *e = parse_expr(stream1);
    return e;
}

Expr* Expr::parse_num(std::istream &in){
    int n = 0;
    bool negative = false;
    
    if(in.peek() == '-'){
        negative = true;
        consume(in, '-');
    }
    while(1){
        int c = in.peek();
        if(isdigit(c)){
            consume(in, c);
            n = n*10 + (c-'0');
        }else
            break;
    }
    if(negative)
        n = -n;
    return new Num(n);
}

Expr* Expr::parse_expr(std::istream &in){
    Expr *e;
    
    e = parse_addend(in);
    
    skip_whitespace(in);
    
    int c = in.peek();
    if(c == '+') {
        consume(in, '+');
        Expr *rhs = parse_expr(in);
        return new Add(e, rhs);
    }else{
        return e;
    }
}

Expr* Expr::parse_addend(std::istream &in){
    Expr *e;
    e = parse_multicand(in);
    
    skip_whitespace(in);
    int c = in.peek();
    if(c == '*') {
        consume(in, '*');
        Expr *rhs = parse_addend(in);
        return new Mult(e, rhs);
    }else{
        return e;
    }
}

Expr* Expr::parse_multicand(std::istream &in){
    skip_whitespace(in);
    
    int c = in.peek();
    if(( c == '-') || isdigit(c))
        return parse_num(in);
    else if(c == '('){
        consume(in, '(');
        Expr *e = parse_expr(in);
        skip_whitespace(in);
        c = in.get();
        if(c != ')')
            throw std::runtime_error("missing closing parenthesis");
        return e;
    } else if(isalpha(c)){
        return parse_var(in);
    } else if(c == '_'){
        return parse_let(in);
    }
    else{
        consume(in, c);
        throw std::runtime_error("invalid input");
    }
}

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

Num::Num(int val) {
    this->val = val;
}

bool Num::equals(Expr *other){
    Num *num = dynamic_cast<Num*>(other);
    if(num == NULL)
        return false;
    else
        return this->val == num->val;
}

int Num::interp(){
    return this->val;
}

bool Num::has_variable(){
    return false;
}

Expr* Num::subst(std::string string,Expr *exp){
    return this;
}

void Num::print(std::ostream& output){
    output << this->val;
}

void Num::pretty_print(std::ostream& output){
    this->pretty_print_at(output, print_group_none, 0);
}

void Num::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    output << this->val;
}


Add::Add(Expr *lhs, Expr *rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

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
    long position = output.tellp();
    long *positionPtr = &position;
    lhs->pretty_print_at(output, print_group_add_or_let, positionPtr);
    output << " + ";
    rhs->pretty_print_at(output, print_group_none, positionPtr);
}

void Add::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    if(mode == print_group_add_or_let || mode == print_group_add || mode == print_group_add_or_mult_or_let){
        output << "(";
        lhs->pretty_print_at(output, print_group_add_or_let, pos);
        output << " + ";
        rhs->pretty_print_at(output, print_group_none, pos);
        output << ")";
    } else{
        lhs->pretty_print_at(output, print_group_add_or_let, pos);
        output << " + ";
        rhs->pretty_print_at(output, print_group_none, pos);
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
    long position = output.tellp();
    long *positionPtr = &position;
    lhs->pretty_print_at(output, print_group_add_or_mult_or_let, positionPtr);
    output << " * ";
    rhs->pretty_print_at(output, print_group_add, positionPtr);
}

void Mult::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
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
    pretty_print_at(output, print_group_none, 0);
};

void Var::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    output << this->var;
}

Let::Let(std::string lhs, Expr *rhs, Expr *body){
    this->lhs = lhs;
    this->rhs = rhs;
    this->body = body;
}

bool Let::equals(Expr *other){
    Let *o = dynamic_cast<Let*>(other);
    if(o == NULL)
        return false;
    else
        return (this->lhs == o->lhs && this->rhs->equals(o->rhs) && this->body->equals(o->body));
}

int Let::interp(){
    int n = this->rhs->interp();
    Num *num = new Num(n);
    Expr *new_body = this->body->subst(this->lhs, num);
    return new_body->interp();
}

bool Let::has_variable(){
    return (this->rhs->has_variable() || this->body->has_variable());
}

Expr* Let::subst(std::string string, Expr* exp){
    std::string new_lhs = this->lhs;
    Expr *new_rhs = this->rhs->subst(string, exp);
    Expr *new_body = this->body;
    if(new_lhs != string){
        new_body = new_body->subst(string, exp);
    }
    return new Let(new_lhs, new_rhs, new_body);
}

void Let::print(std::ostream& output){
    output << "(_let ";
    output << lhs;
    output << "=";
    rhs->print(output);
    output << " _in ";
    body->print(output);
    output << ")";
}

void Let::pretty_print(std::ostream& output){
    output << "_let " << this->lhs << " = ";
    this->rhs->pretty_print_at(output, print_group_none, 0);
    output << "\n";
    long new_pos = output.tellp();
    long *positionPtr = &new_pos;
    output << "_in  ";
    this->body->pretty_print_at(output, print_group_none, positionPtr);
    
}
void Let::pretty_print_at(std::ostream& output, print_mode_t mode, long *pos){
    if(mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
        output << "(";
        long letPos = output.tellp();
        output << "_let ";
        output << this->lhs << " = ";
        this->rhs->pretty_print_at(output, print_group_none, pos);
        output << "\n";
        long spaces = letPos - *pos;
        *pos = output.tellp();
        for(int i = 0; i < spaces; i++){
            output << " ";
        }
        output << "_in  ";
        body->pretty_print_at(output, print_group_none, pos);
        if(mode == print_group_add_or_let || mode == print_group_add_or_mult_or_let)
            output << ")";
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
    Let *nullLet = NULL;
    
    Let *let1 = new Let("x", new Num(2), new Add(new Num(5), new Var("x")));
    Let *let2 = new Let("x", new Num(2), new Add(new Num(5), new Var("x")));
    //Let *let3 = new Let("x", new Num(2), new Add(new Num(5), new Num(2)));
    std::string pp_let1 = "_let x = 5\n        _in  (_let y = 3\n              _in  y + 2) + x";
        
    //Always substitue RHS. Body changes iff the variable we are replacing and the bound variable are different
      //
      //No Change
      //_let x = 1
      //_in x + 2 ->subst(x, y+3)
      Expr *let3 = new Let("x", new Num(1), new Add(new Var("y"), new Num(2)));
      CHECK((let3)->subst("x", new Add(new Var("y"), new Num(3)))->equals(let3));
      //Only change RHS
      //_let x = x
      //_in x + 2 ->subst(x, 5)
      Expr *let4 = new Let("x",
                 new Var("x"),
                 new Add(new Var("x"), new Num(2)));
      CHECK((let4->subst("x", new Num(5))
          ->equals(new Let("x",
                  new Num(5),
                  new Add (new Var("x"), new Num(2))))));
      //Only change Body
      //_let x = 8
      //_in x+y-> subst(“y”, 9)
      // =
      //_let x = 8
      //_in x+y
      Expr *let5 = new Let ("x",
                 new Num(8),
                 new Add (new Var("x"), new Var("y")));
      CHECK((let5-> subst("y", new Num(9)))
         ->equals(new Let ("x",
                  new Num(8),
                  new Add (new Var("x"), new Num(9)))));
      //Change RHS and Body
      //_let x = y
      //_in x+y-> subst(“y”, 9)
      // =
      //_let x = 9
      //_in x+9
      Expr *let6 = new Let ("x",
                 new Var ("y"),
                 new Add (new Var("x"), new Var("y")));
      CHECK((let6-> subst("y", new Num(9)))
         ->equals(new Let ("x",
                  new Num(9),
                  new Add (new Var("x"), new Num(9)))));
    
    CHECK((new Add(new Mult(new Num(5), new Let("x", new Num(5), new Var("x"))), new Num (1)))->interp() == 26);
    CHECK((new Add(new Mult(new Num(5), new Let("x", new Num(5), new Var("x"))), new Num (1)))->pp_to_string() == "5 * (_let x = 5\n     _in  x) + 1");
    
    CHECK((new Mult(new Num(5), new Add(new Let("x", new Num(5), new Var("x")), new Num (1))))->interp() == 30);
    CHECK((new Mult(new Num(5), new Add(new Let("x", new Num(5), new Var("x")), new Num (1))))->pp_to_string() == "5 * ((_let x = 5\n      _in  x) + 1)");
    
    CHECK((new Mult(new Num(5), new Let("x", new Num(5), new Add(new Var("x"), new Num (1)))))->pp_to_string() == "5 * _let x = 5\n    _in  x + 1");
    CHECK((new Mult(new Num(5), new Let("x", new Num(5), new Add(new Var("x"), new Num (1)))))->interp() == 30);
    

    CHECK ((new Mult( new Num (5), (new Let("x", new Num(5), new Let("y", new Num(3), new Let( "z", new Num(1), new Add (new Var("z"), new Num(4))))))))->pp_to_string() == "5 * _let x = 5\n    _in  _let y = 3\n         _in  _let z = 1\n              _in  z + 4");
    //Let pretty print
        CHECK ((new Let("x", new Num(5), new Let("x", new Num(1), new Add( new Var("x"), new Num(3)))))->pp_to_string() == "_let x = 5\n_in  _let x = 1\n     _in  x + 3");
    
        pp_let1 = "_let x = 5\n_in  x + 1";
        CHECK ((new Let("x", new Num(5), new Add(new Var("x"), new Num(1))))
               ->pp_to_string() == pp_let1);
        pp_let1 = "_let x = 5\n_in  _let x = 2\n     _in  x + 3";
        CHECK ((new Let("x", new Num(5), new Let("x", new Num(2), new Add( new Var("x"), new Num(3)))))
               ->pp_to_string() == pp_let1);
        pp_let1 = "_let x = 5\n_in  (_let y = 3\n      _in  y + 2) + x";
        CHECK ((new Let("x", new Num(5), new Add(new Let("y", new Num(3), new Add(new Var("y"), new Num(2))), new Var("x"))))
               ->pp_to_string() == pp_let1);
    
    //CHECK(ppLet->pp_to_string() == pp_let1);
    CHECK(let1->equals(let2) == true);
    CHECK(let1->equals(nullLet) == false);
    CHECK(let1->has_variable() == true);
    CHECK(let2->interp() == 7);
    CHECK(let2->subst("x", num3)->interp() == 7);
    
    CHECK_THROWS_WITH((new Let("x", new Var("t"), new Add(new Var("x"), new Num(1))))->interp(), "variable has no value");
    CHECK((new Let("x", new Num(5), new Add(new Var("x"), new Num(1))))->interp() == 6);
        CHECK((new Let("x", new Add( new Num(5), new Num (2)), new Add(new Var("x"), new Num(1))))->interp() == 8);
    
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
    //myVar2->print(os);
    myVar2->print(os);
    std::cout << "=y\n";
    
    std::string testString = "(_let x=5 _in ((_let y=3 _in (y+2))+x))";
    
    CHECK ((new Let("x", new Num(5), new Add(new Var("x"), new Num(1))))->to_string() == "(_let x=5 _in (x+1))");
        CHECK ((new Let("x", new Num(5), new Add(new Let("y", new Num(3), new Add(new Var("y"), new Num(2))), new Var("x"))))->to_string() == "(_let x=5 _in ((_let y=3 _in (y+2))+x))");

    //new Let("x", new Num(5), new Let("y", new Add(new Var("y"), new Num(2)));
    //Test 11 [Add (let, let)]
        testString = "(_let x = 1\n _in  x + 2) + _let y = 3\n               _in  y + 4";
        CHECK((new Add( new Let( "x", new Num(1), new Add( new Var("x"), new Num(2))), new Let( "y", new Num(3), new Add( new Var("y"), new Num(4)))))->pp_to_string() == testString);
    

//    Var *numX = new Var("X");
//    Var *numY = new Var("Y");
//    Add *add3_3 = new Add(num3,num3);
//    Add *add3_2 = new Add(num3,num2);
//    Mult *mult3_2 = new Mult(num3,num2);
//    Add *add1_2_3 = new Add(new Num(1), new Add(new Num(2), new Num(3)));
//    Add *add1_2then3 = new Add(new Add(new Num(1), new Num(2)), new Num(3));
//    Add *mult1_2_3 = new Add(new Num(1), new Mult(new Num(2), new Num(3)));
//    Mult *mult1_2then3 = new Mult(new Add(new Num(1), new Num(2)), new Num(3));
//    Mult *mult1then2_3 = new Mult(new Num(1),new Add( new Num(2), new Num(3)));
//    Mult *mult3_3thenM3_2 = new Mult(add3_3,mult3_2);
//    Mult *mult3_2thenMadd3_2 = new Mult(mult3_2,add3_2);
//    num3->pretty_print(os);
//    num2->pretty_print(os);
//    numY->pretty_print(os);
//    numX->pretty_print(os);
//    add3_3->pretty_print(os);
//    mult3_2->print(os);
//    mult3_2->print(os);
//    printf("\n");
//    add1_2_3->pretty_print(os);
//    printf("\n");
//    add1_2then3->pretty_print(os);
//    printf("\n");
//    mult1_2_3->pretty_print(os);
//    printf("\n");
//    mult1_2then3->pretty_print(os);
//    printf("\n");
//    mult1then2_3->pretty_print(os);
//    printf("\n");
//    mult3_3thenM3_2->pretty_print(os);
//    printf("\n");
//    mult3_2thenMadd3_2->pretty_print(os);

    
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
    
    
    
    testString = "5";
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
