//
//  pointer.cpp
//  msdscript
//
//  Created by Nick Beckley on 3/16/21.
//

#ifndef pointer_h
#define pointer_h

#include <memory>

#define USE_PLAIN_POINTERS 1
#if USE_PLAIN_POINTERS

# define NEW(T)    new T
# define PTR(T)    T*
# define CAST(T)   dynamic_cast<T*>
# define CLASS(T)  class T
# define THIS      this

#else

# define NEW(T)    std::make_shared<T>
# define PTR(T)    std::shared_ptr<T>
# define CAST(T)   std::dynamic_pointer_cast<T>
# define CLASS(T)  class T : public std::enable_shared_from_this<T>
# define THIS      shared_from_this()

#endif

#endif
