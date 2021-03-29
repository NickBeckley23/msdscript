//
//  Env.hpp
//  msdscript
//
//  Created by Nick Beckley on 3/29/21.
//

#ifndef Env_h
#define Env_h

#include <stdio.h>
#include <string>
#include "pointer.h"

class Val;

class Env {
public:
    static PTR(Env) empty;
    virtual PTR(Val) lookup(std::string find_name) = 0;
};

class EmptyEnv : public Env{
public:
    EmptyEnv();
    PTR(Val) lookup(std::string find_name);
};

class ExtendedEnv : public Env{
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;
    
    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest);
    
    PTR(Val) lookup(std::string find_name);
};

#endif /* Env_h */
