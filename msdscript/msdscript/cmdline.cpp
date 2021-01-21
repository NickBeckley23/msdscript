//
//  cmdline.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/19/21.
//

#include "cmdline.h"
#include <iostream>


void use_arguments(int argc,char * argv[]){
    if(argc == 1) exit(1);
    bool testSeen = false;
    for(int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if(arg == "--help"){
            std::cout << "Arguments allowed: --help -- test\n";
            exit(0);
        }else if(arg == "--test" && testSeen == false){
            std::cout << "Tests Passed\n";
            testSeen = true;
        }else if(arg == "--test" && testSeen == true){
            std::cerr << "Tests already passed yo\n";
            exit(1);
        }else{
            std::cerr << "Invalid argument\n";
            exit(1);
        }
    }
}
