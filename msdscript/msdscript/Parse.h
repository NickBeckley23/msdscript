//
//  Parse.hpp
//  msdscript
//
//  Created by Nick Beckley on 2/16/21.
//

#ifndef Parse_hpp
#define Parse_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include "Expr.h"

   
void consume(std::istream &in, int expect);

void skip_whitespace(std::istream &in);

void parse_keyword(std::istream &in, std::string keyword);

Expr *parse_expr(std::istream &in);
Expr *parse_num(std::istream &in);
Expr *parse_addend(std::istream &in);
Expr *parse_multicand(std::istream &in);
Expr *parse(std::istream &in);
Expr *parse_str(std::string s);
Expr *parse_var(std::istream &in);
Expr *parse_let(std::istream &in);

#endif /* Parse_hpp */
