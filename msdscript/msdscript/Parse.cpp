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
#include "Step.h"
#include "Cont.h"

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

PTR(Expr) parse_var(std::istream &in){
    skip_whitespace(in);
    std::string var = "";
    int c = in.peek();
    while(isalpha(c)){
        consume(in, c);
        var += c;
        c = in.peek();
    }
    return NEW(VarExpr)(var);
}

PTR(Expr) parse(std::istream &in){
    PTR(Expr)e = parse_expr(in);
    return e;
}

PTR(Expr) parse_str(std::string s){
    std::istringstream stream1(s);
    PTR(Expr)e = parse_expr(stream1);
    return e;
}

PTR(Expr) parse_num(std::istream &in){
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
    return NEW(NumExpr)(n);
}

PTR(Expr) parse_expr(std::istream &in){
    PTR(Expr)e = parse_comparg(in);
    
    skip_whitespace(in);
    
    int c = in.peek();
    if(c == '=') {
        consume(in, '=');
        consume(in, '=');
        PTR(Expr)rhs = parse_expr(in);
        return NEW(EqExpr)(e, rhs);
    }else{
        return e;
    }
}

PTR(Expr) parse_comparg(std::istream &in){
    PTR(Expr)exp = parse_addend(in);
    skip_whitespace(in);
    int c = in.peek();
    if(c == '+'){
        consume(in, '+');
        PTR(Expr) o = parse_comparg(in);
        return NEW(AddExpr)(exp, o);
    }else{
        return exp;
    }
}

PTR(Expr) parse_addend(std::istream &in){
    PTR(Expr)e = parse_multicand(in);
    skip_whitespace(in);
    int c = in.peek();
    if(c == '*') {
        consume(in, '*');
        PTR(Expr)rhs = parse_addend(in);
        return NEW(MultExpr)(e, rhs);
    }else{
        return e;
    }
}

PTR(Expr) parse_multicand(std::istream &in) {
    PTR(Expr)expr = parse_inner(in);
    while (in.peek() == '(') {
        consume(in, '(');
        PTR(Expr)arg = parse_expr(in);
        consume(in, ')');
        skip_whitespace(in);
        expr = NEW(CallExpr)(expr, arg);
    }
    return expr;
}

PTR(Expr) parse_inner(std::istream &in){
    skip_whitespace(in);
    int c = in.peek();
    if(( c == '-') || isdigit(c)){
        return parse_num(in);
    }
    else if(c == '('){
        consume(in, '(');
        PTR(Expr)e = parse_expr(in);
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

PTR(Expr) parse_let(std::istream &in){
    PTR(Expr)v = parse_var(in);
    skip_whitespace(in);
    consume(in, '=');
    PTR(Expr)rhs = parse_expr(in);
    skip_whitespace(in);
    std::string kw = parse_keyword(in);
    if(kw != "in")
        throw std::runtime_error("invalid keyword parsed");
    PTR(Expr)body = parse_expr(in);
    return NEW(LetExpr)(v->pp_to_string(), rhs, body);
}

PTR(Expr) parse_if(std::istream &in){
    PTR(Expr) test = parse_expr(in);
    skip_whitespace(in);
    if(parse_keyword(in) != "then")
        throw std::runtime_error("invalid then keyword");
    skip_whitespace(in);
    PTR(Expr)then = parse_expr(in);
    skip_whitespace(in);
    if(parse_keyword(in) != "else")
        throw std::runtime_error("invalid else keyword");
    skip_whitespace(in);
    PTR(Expr)else_part = parse_expr(in);
    return NEW(IfExpr)(test, then, else_part);
}

PTR(Expr) parse_function(std::istream &in){
    skip_whitespace(in);
    consume(in, '(');
    PTR(Expr)v = parse_expr(in);
    consume(in, ')');
    skip_whitespace(in);
    PTR(Expr)body = parse_expr(in);
    return NEW(FunExpr)(v->to_string(), body);
}

PTR(Expr) parse_false(std:: istream &in){
    return NEW(BoolExpr)(false);
}

PTR(Expr) parse_true(std::istream &in){
    return NEW(BoolExpr)(true);
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
    CHECK(parse(in)->equals(NEW(BoolExpr)(true)));
}

TEST_CASE ("Parse Num") {
    PTR(Expr)n = parse_str("100");
    CHECK( n->equals(NEW(NumExpr)(100)));
}

TEST_CASE ("Parse True") {
    PTR(Expr)t = parse_str("_true");
    PTR(Expr)f = parse_str("_false");
    CHECK( t->equals(NEW(BoolExpr)(true)));
    CHECK( f->equals(NEW(BoolExpr)(false)));
}

TEST_CASE ("Parse False") {
    PTR(Expr)t = parse_str("_true");
    PTR(Expr)f = parse_str("_false");
    CHECK( t->equals(NEW(BoolExpr)(true)));
    CHECK( f->equals(NEW(BoolExpr)(false)));
}

TEST_CASE ("Parse Var") {
    PTR(Expr)var = parse_str("hello");
    CHECK( var->equals(NEW(VarExpr)("hello")));
    PTR(Expr)v = parse_str("y");
    CHECK(v->equals(NEW(VarExpr)("y")));
    v = parse_str("nick");
    CHECK(v->equals(NEW(VarExpr)("nick")));
}

TEST_CASE("Parse Let"){
    std::string testString = "_let x = 5\n_in  x + 1";
    PTR(Expr)e = parse_str(testString);
    CHECK ((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))
           ->equals(e));
    PTR(LetExpr)let1 = NEW(LetExpr)("x", NEW(NumExpr)(5), (NEW(AddExpr) (NEW(VarExpr)("x"), NEW(NumExpr) (4))));
    e = NEW(LetExpr)("x", NEW(NumExpr)(5), (NEW(AddExpr) (NEW(VarExpr)("x"), NEW(NumExpr)(4))));
    CHECK((let1->equals(e))==true);
    CHECK((let1->equals(NEW(MultExpr)(NEW(NumExpr)(0),NEW(NumExpr)(1))))==false);
    testString = "(_let x=2 _in x+1)";
    CHECK(parse_str(testString)->equals(NEW(LetExpr)("x", NEW(NumExpr)(2), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1)))));
    testString = "(_let x=2 _i x+1)";
    CHECK_THROWS_WITH(parse_str(testString), "invalid keyword parsed");
}

TEST_CASE("Parse If"){
    std::string testString = "_if 4 + 1\n_then 2\n_else 3";
    CHECK((NEW(IfExpr)(NEW(AddExpr)(NEW(NumExpr)(4), NEW(NumExpr)(1)), NEW(NumExpr)(2), NEW(NumExpr)(3)))->pp_to_string() == testString);
    testString = "_if 1==1 _then 1 _else 2";
    CHECK(parse_str(testString)->equals(NEW(IfExpr)(NEW(EqExpr)(NEW(NumExpr)(1), NEW(NumExpr)(1)), NEW(NumExpr)(1), NEW(NumExpr)(2))) == true);
    testString = "X";
    CHECK(parse_str(testString)->equals(NEW(VarExpr)("X")) == true);
    testString = "_if 1==1 _the 1 _else 2";
    CHECK_THROWS_WITH((parse_str(testString)->equals(NEW(IfExpr)(NEW(EqExpr)(NEW(NumExpr)(1), NEW(NumExpr)(1)), NEW(NumExpr)(1), NEW(NumExpr)(2)))), "invalid then keyword");
    testString = "_if 1==1 _then 1 _els 2";
    CHECK_THROWS_WITH((parse_str(testString)->equals(NEW(IfExpr)(NEW(EqExpr)(NEW(NumExpr)(1), NEW(NumExpr)(1)), NEW(NumExpr)(1), NEW(NumExpr)(2)))), "invalid else keyword");
}

TEST_CASE("Parse Multicand"){
    std::string testString = "1 * 2";
    CHECK(parse_str(testString)->equals(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) == true);
    testString = "(1*2)";
    CHECK(parse_str(testString)->equals(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2))) == true);
    
}

TEST_CASE("Parse Function"){
    CHECK((parse_str("_let f = _fun (x) x+1 _in f(5)")->equals(NEW(LetExpr)("f", NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))), NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(5))))));
    CHECK((parse_str("_let f = _fun (x) _true _in f(5)")->equals(NEW(LetExpr)("f", NEW(FunExpr)("x", NEW(BoolExpr)(true)), NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(5))))));
}


TEST_CASE("PARSE"){
    std::stringstream ss;
    ss.str("_notlet");
    CHECK((parse_str("1")->equals(NEW(NumExpr)(1))));
    CHECK((parse_str("-1")->equals(NEW(NumExpr)(-1))));
    ss.str("hello");
    CHECK_THROWS_WITH(parse_keyword(ss), "consume mismatch");
    ss.str("x");
    CHECK_THROWS_WITH(consume(ss, 1), "consume mismatch");
}
