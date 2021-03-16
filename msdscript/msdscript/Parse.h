//
//  Parse.hpp
//  msdscript
//
//  Created by Nick Beckley on 2/16/21.
//

#ifndef Parse_h
#define Parse_h

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include "Expr.h"
#include "pointer.h"

   
void consume(std::istream &in, int expect);

void skip_whitespace(std::istream &in);

std::string parse_keyword(std::istream &in);

PTR(Expr) parse_expr(std::istream &in);
PTR(Expr) parse_num(std::istream &in);
PTR(Expr) parse_comparg(std::istream &in);
PTR(Expr) parse_addend(std::istream &in);
PTR(Expr) parse_multicand(std::istream &in);
PTR(Expr) parse_function(std::istream &in);
PTR(Expr) parse_inner(std::istream &in);
PTR(Expr) parse_str(std::string s);
PTR(Expr) parse_var(std::istream &in);
PTR(Expr) parse_let(std::istream &in);
PTR(Expr) parse_if(std::istream &in);
PTR(Expr) parse_false(std::istream &in);
PTR(Expr) parse_true(std::istream &in);

#endif /* Parse_hpp */
