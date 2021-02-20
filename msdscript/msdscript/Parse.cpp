//
//  Parse.cpp
//  msdscript
//
//  Created by Nick Beckley on 2/16/21.
//

#include "Parse.h"
#include "Expr.h"
#include "catch.h"

void consume(std::istream &in, int expect){
    int c = in.get();
    if(c != expect)
        throw std::runtime_error("consume mismatch");
}

void skip_whitespace(std::istream &in){
    while(1){
        int c = in.peek();
        if(!isspace(c))
            break;
        consume(in, c);
    }
}

void parse_keyword(std::istream &in, std::string keyword){
    std::string word = "";
    for(int i = 0; i < keyword.length(); i++){
        word += in.get();
    }
    if(word != keyword)
        throw std::runtime_error("keyword not found");
    return;
}

Expr* parse_var(std::istream &in){
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

Expr* parse_let(std::istream &in){
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

//Expr* parse(std::istream &in){
//    Expr *e = parse_expr(in);
//    return e;
//}

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
    return new Num(n);
}

Expr* parse_expr(std::istream &in){
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

Expr* parse_addend(std::istream &in){
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

Expr* parse_multicand(std::istream &in){
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

TEST_CASE("PARSE"){
    //std::ostream& os = std::cout;
    std::stringstream ss;
    std::string pp_let1 = "_let x = 5\n        _in  (_let y = 3\n              _in  y + 2) + x";
    ss.str("_notlet");
    CHECK((parse_str("1")->equals(new Num(1))));
    CHECK((parse_str("-1")->equals(new Num(-1))));
    CHECK_THROWS_WITH((parse_str("(1*1")->interp()), "missing closing parenthesis");
    CHECK_THROWS_WITH((parse_str("^1*1")->interp()), "invalid input");
    CHECK_THROWS_WITH(parse_keyword(ss, "_let"), "keyword not found");
    ss.str("x");
    CHECK_THROWS_WITH(consume(ss, 1), "consume mismatch");
    ss.str(pp_let1);
    CHECK((parse_let(ss)->interp()==10));
}
