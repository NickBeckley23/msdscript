//
//  cmdline.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/19/21.
//

#define CATCH_CONFIG_RUNNER
#include "cmdline.h"
#include "Expr.h"
#include "Parse.h"
#include <iostream>

void use_arguments(int argc,char * argv[]){
    if(argc == 1) exit(1);
    bool testSeen = false;
    for(int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if(arg == "--help"){
            std::cout << "Arguments allowed: --help --test --interp --print --pretty_print\n";
            exit(0);
        }else if(arg == "--test" && testSeen == false){
            int fail = Catch::Session().run(1, argv);
            if(fail != 0) exit(1);
            std::cout << "Tests Passed\n";
            testSeen = true;
        }else if(arg == "--test" && testSeen == true){
            std::cerr << "Tests already passed yo\n";
            exit(1);
        }else if(arg == "--interp"){
            Expr *e = parse_expr(std::cin);
//            e = Parse::
            try{
            std::cout << e->interp();
            exit(0);
            }catch(std::runtime_error& e){
                std::cout << "bad input could not interp, exiting";
                exit(1);
            }
        }else if(arg == "--print"){
            Expr *e = nullptr;
            e = parse_expr(std::cin);
            e->print(std::cout);
            exit(0);
        }else if(arg == "--pretty-print"){
            Expr *e = nullptr;
            e = parse_expr(std::cin);
            std::cout << e->pp_to_string();
            exit(0);
        }else{
            std::cerr << "Invalid argument\n";
            exit(1);
        }
    }
}
