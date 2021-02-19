//
//  main.cpp
//  test_msdscript
//
//  Created by Nick Beckley on 2/19/21.
//

#include <iostream>
#include "exec.hpp"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


static std::string random_expr_string();

int main(int argc, const char * argv[]) {
    srand(clock());
    
    const char * const interp_argv[] = { argv[1], "--interp" };
    const char * const print_argv[] = { argv[1], "--print" };
    const char * const pprint_argv[] = {argv[1], "--pretty-print"};
    
    if(argc == 2){
        
    for (int i = 0; i < 100; i++) {
        std::string in = random_expr_string();
        std::cout << "Trying " << in << "\n";
        
        ExecResult interp_result = exec_program(2, interp_argv, in);
        ExecResult print_result = exec_program(2, print_argv, in);
        
        ExecResult interp_again_result = exec_program(2, interp_argv, print_result.out);
        
        if (interp_again_result.out != interp_result.out)
            throw std::runtime_error("different result for printed");
        }
    }
    else if(argc == 3) {
        const char * const interp1_argv[] = { argv[2], "--interp" };
        const char * const print1_argv[] = { argv[2], "--print" };
        const char * const pprint1_argv[] = { argv[2], "--pretty-print" };
        
        for (int i = 0; i < 100; i++) {
        std::string in = random_expr_string();
        std::cout << "Trying " << in << "\n";
        ExecResult interp_result = exec_program(2, interp_argv, in);
        ExecResult print_result = exec_program(2, print_argv, in);
        ExecResult pprint_result = exec_program(2, pprint_argv, in);
        
        ExecResult interp1_result = exec_program(2, interp1_argv, in);
        ExecResult print1_result = exec_program(2, print1_argv, in);
        ExecResult pprint1_result = exec_program(2, pprint1_argv, in);
        
            if (interp_result.out != interp1_result.out)
                throw std::runtime_error("different result for interp");
            if (print_result.out != print1_result.out)
                throw std::runtime_error("different result for print");
            if (pprint_result.out != pprint1_result.out)
                throw std::runtime_error("different result for pretty-print");
        }
    }
    return 0;
}

std::string random_expr_string() {
    int n = (rand() % 10);
    if (n < 5){
        return std::to_string(rand());
    }
    else if (n < 7)
        if(n == 5)
            return "(" + random_expr_string() + "+" + random_expr_string() + ")";
        else
            return random_expr_string() + "+" + random_expr_string();
    else if (n < 9)
        if(n == 7)
            return "(" + random_expr_string() + "*" + random_expr_string() + ")";
        else
            return random_expr_string() + "*" + random_expr_string();
    else
        return "_let x = " + random_expr_string() + "_in " + random_expr_string();
}
