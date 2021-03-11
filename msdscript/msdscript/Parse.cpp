//
//  Parse.cpp
//  msdscript
//
//  Created by Nick Beckley on 2/16/21.
//

#include "Parse.h"
#include "Expr.h"
#include "catch.h"
#include "Val.h"

void consume(std::istream &in, int expect){
    int c = in.get();
    if(c != expect)
        throw std::runtime_error("consume mismatch");
}

void skip_whitespace(std::istream &in){
    while(1){
        int c = in.peek();
        if(!isspace(c)){
            break;
        }
        consume(in, c);
    }
}

std::string parse_keyword(std::istream &in){
    std::string word = "";
    consume(in, '_');
    int c = in.peek();
    while(isalpha(c)){
        c = in.get();
        word += c;
        c = in.peek();
    }
    return word;
}

Expr* parse_var(std::istream &in){
    skip_whitespace(in);
    std::string var = "";
    int c = in.peek();
    while(isalpha(c)){
        consume(in, c);
        var += c;
        c = in.peek();
    }
    return new VarExpr(var);
}

Expr* parse(std::istream &in){
    Expr *e = parse_expr(in);
    return e;
}

Expr* parse_str(std::string s){
    std::istringstream stream1(s);
    Expr *e = parse_expr(stream1);
    return e;
}

Expr* parse_num(std::istream &in){
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
    return new NumExpr(n);
}

Expr* parse_expr(std::istream &in){
    Expr *e = parse_comparg(in);
    
    skip_whitespace(in);
    
    int c = in.peek();
    if(c == '=') {
        consume(in, '=');
        consume(in, '=');
        Expr *rhs = parse_expr(in);
        return new EqExpr(e, rhs);
    }else{
        return e;
    }
}

Expr* parse_comparg(std::istream &in){
    Expr *exp = parse_addend(in);
    skip_whitespace(in);
    int c = in.peek();
    if(c == '+'){
        consume(in, '+');
        Expr* o = parse_comparg(in);
        return new AddExpr(exp, o);
    }else{
        return exp;
    }
}

Expr* parse_addend(std::istream &in){
    Expr *e = parse_multicand(in);
    skip_whitespace(in);
    int c = in.peek();
    if(c == '*') {
        consume(in, '*');
        Expr *rhs = parse_addend(in);
        return new MultExpr(e, rhs);
    }else{
        return e;
    }
}

Expr* parse_multicand(std::istream &in) {
    Expr *expr = parse_inner(in);
    while (in.peek() == '(') {
        consume(in, '(');
        Expr *arg = parse_expr(in);
        consume(in, ')');
        skip_whitespace(in);
        expr = new CallExpr(expr, arg);
    }
    return expr;
}

Expr* parse_inner(std::istream &in){
    skip_whitespace(in);
    int c = in.peek();
    if(( c == '-') || isdigit(c)){
        return parse_num(in);
    }
    else if(c == '('){
        consume(in, '(');
        Expr *e = parse_expr(in);
        skip_whitespace(in);
        c = in.get();
        if(c != ')')
            throw std::runtime_error("missing closing parenthesis");
        return e;
        
    } else if(isalpha(c))
        return parse_var(in);
    else if(c == '_'){
        std::string kw = parse_keyword(in);
        if(kw == "if")
            return parse_if(in);
        else if(kw == "false")
            return parse_false(in);
        else if(kw == "true")
            return parse_true(in);
        else if(kw == "let")
            return parse_let(in);
        else if(kw == "fun")
            return parse_function(in);
    }
    consume(in, c);
    throw std::runtime_error("invalid input");
}

Expr* parse_let(std::istream &in){
    Expr *v = parse_var(in);
    skip_whitespace(in);
    consume(in, '=');
    Expr *rhs = parse_expr(in);
    skip_whitespace(in);
    std::string kw = parse_keyword(in);
    if(kw != "in")
        throw std::runtime_error("invalid keyword parsed");
    Expr *body = parse_expr(in);
    return new LetExpr(v->pp_to_string(), rhs, body);
}

Expr* parse_if(std::istream &in){
    Expr* test = parse_expr(in);
    skip_whitespace(in);
    if(parse_keyword(in) != "then")
        throw std::runtime_error("invalid then keyword");
    skip_whitespace(in);
    Expr *then = parse_expr(in);
    skip_whitespace(in);
    if(parse_keyword(in) != "else")
        throw std::runtime_error("invalid else keyword");
    skip_whitespace(in);
    Expr *else_part = parse_expr(in);
    return new IfExpr(test, then, else_part);
}

Expr* parse_function(std::istream &in){
    skip_whitespace(in);
    consume(in, '(');
    Expr *v = parse_expr(in);
    consume(in, ')');
    skip_whitespace(in);
    Expr *body = parse_expr(in);
    return new FunExpr(v->to_string(), body);
}

Expr* parse_false(std:: istream &in){
    return new BoolExpr(false);
}

Expr* parse_true(std::istream &in){
    return new BoolExpr(true);
}

TEST_CASE ("Skip_Whitespace") {
    std::stringstream in(" testing");
    skip_whitespace(in);
    std::string in_str = "";
    int c = in.peek();
    while (c != -1) {
        c = in.get();
        in_str += c;
        c = in.peek();
    }
    CHECK(in_str == "testing");
}

TEST_CASE ("Consume") {
    std::stringstream in("nick");
    consume(in, 'n');
    int c = in.peek();
    CHECK( c == 'i');
    consume(in, 'i');
    c = in.peek();
    CHECK( c == 'c');
    CHECK_THROWS_WITH(consume(in, 'k'), "consume mismatch");
}

TEST_CASE ("Parse Keyword") {
    std::stringstream in("_let");
    std::string let = parse_keyword(in);
    CHECK(let == "let");
}

TEST_CASE ("Parse") {
    std::stringstream in("_true");
    CHECK(parse(in)->equals(new BoolExpr(true)));
}

TEST_CASE ("Parse Num") {
    Expr *n = parse_str("100");
    CHECK( n->equals(new NumExpr(100)));
}

TEST_CASE ("Parse True") {
    Expr *t = parse_str("_true");
    Expr *f = parse_str("_false");
    CHECK( t->equals(new BoolExpr(true)));
    CHECK( f->equals(new BoolExpr(false)));
}

TEST_CASE ("Parse False") {
    Expr *t = parse_str("_true");
    Expr *f = parse_str("_false");
    CHECK( t->equals(new BoolExpr(true)));
    CHECK( f->equals(new BoolExpr(false)));
}

TEST_CASE ("Parse Var") {
    Expr *var = parse_str("hello");
    CHECK( var->equals(new VarExpr("hello")));
    Expr *v = parse_str("y");
    CHECK(v->equals(new VarExpr("y")));
    v = parse_str("nick");
    CHECK(v->equals(new VarExpr("nick")));
}

TEST_CASE("Parse Let"){
    std::string testString = "_let x = 5\n_in  x + 1";
    Expr *e = parse_str(testString);
    CHECK ((new LetExpr("x", new NumExpr(5), new AddExpr(new VarExpr("x"), new NumExpr(1))))
           ->equals(e));
    LetExpr *let1 = new LetExpr("x", new NumExpr(5), (new AddExpr (new VarExpr("x"), new NumExpr (4))));
    e = new LetExpr("x", new NumExpr(5), (new AddExpr (new VarExpr("x"), new NumExpr(4))));
    CHECK((let1->equals(e))==true);
    CHECK((let1->equals(new MultExpr(new NumExpr(0),new NumExpr(1))))==false);
    testString = "(_let x=2 _in x+1)";
    CHECK(parse_str(testString)->equals(new LetExpr("x", new NumExpr(2), new AddExpr(new VarExpr("x"), new NumExpr(1)))));
    testString = "(_let x=2 _i x+1)";
    CHECK_THROWS_WITH(parse_str(testString), "invalid keyword parsed");
}

TEST_CASE("Parse If"){
    std::string testString = "_if 4 + 1\n_then 2\n_else 3";
    CHECK((new IfExpr(new AddExpr(new NumExpr(4), new NumExpr(1)), new NumExpr(2), new NumExpr(3)))->pp_to_string() == testString);
    testString = "_if 1==1 _then 1 _else 2";
    CHECK(parse_str(testString)->equals(new IfExpr(new EqExpr(new NumExpr(1), new NumExpr(1)), new NumExpr(1), new NumExpr(2))) == true);
    testString = "X";
    CHECK(parse_str(testString)->equals(new VarExpr("X")) == true);
    testString = "_if 1==1 _the 1 _else 2";
    CHECK_THROWS_WITH((parse_str(testString)->equals(new IfExpr(new EqExpr(new NumExpr(1), new NumExpr(1)), new NumExpr(1), new NumExpr(2)))), "invalid then keyword");
    testString = "_if 1==1 _then 1 _els 2";
    CHECK_THROWS_WITH((parse_str(testString)->equals(new IfExpr(new EqExpr(new NumExpr(1), new NumExpr(1)), new NumExpr(1), new NumExpr(2)))), "invalid else keyword");
}

TEST_CASE("Parse Multicand"){
    std::string testString = "1 * 2";
    CHECK(parse_str(testString)->equals(new MultExpr(new NumExpr(1), new NumExpr(2))) == true);
    testString = "(1*2)";
    CHECK(parse_str(testString)->equals(new MultExpr(new NumExpr(1), new NumExpr(2))) == true);
    
}

TEST_CASE("Parse Function"){
    CHECK((parse_str("_let f = _fun (x) x+1 _in f(5)")->equals(new LetExpr("f", new FunExpr("x", new AddExpr(new VarExpr("x"), new NumExpr(1))), new CallExpr(new VarExpr("f"), new NumExpr(5))))));
    CHECK((parse_str("_let f = _fun (x) _true _in f(5)")->equals(new LetExpr("f", new FunExpr("x", new BoolExpr(true)), new CallExpr(new VarExpr("f"), new NumExpr(5))))));
}


TEST_CASE("PARSE"){
    std::stringstream ss;
    ss.str("_notlet");
    CHECK((parse_str("1")->equals(new NumExpr(1))));
    CHECK((parse_str("-1")->equals(new NumExpr(-1))));
    CHECK_THROWS_WITH((parse_str("(1*1")->interp()), "missing closing parenthesis");
    CHECK_THROWS_WITH((parse_str("^1*1")->interp()), "invalid input");
    ss.str("hello");
    CHECK_THROWS_WITH(parse_keyword(ss), "consume mismatch");
    ss.str("x");
    CHECK_THROWS_WITH(consume(ss, 1), "consume mismatch");
}
