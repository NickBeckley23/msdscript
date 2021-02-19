//
//  main.cpp
//  msdscript
//
//  Created by Nick Beckley on 1/19/21.
//


#include <iostream>
#include "cmdline.h"
#include "Expr.h"


int main(int argc,char * argv[]) {
    try{
        use_arguments(argc,argv);
        return 0;
    } catch(std::runtime_error e){
        std::cerr << e.what();
        exit(1);
    }
}
