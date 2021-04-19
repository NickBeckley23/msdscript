# MSDScript

## Table of Contents
+ [About](#about)
+ [Installation](#installation)
+ [Getting Started](#gettingstarted)
+ [Grammar](#grammar)

## About <a name = "about"></a>
MSDScript is a simple scripting language and interpreter which can be run on the CLI or included as a static Library in your own C++ program. The following operations are supported in MSDScript:

- Addition
- Multiplication
- Variables
- Functions
- Booleans
- If/Else Statements

## Getting Started <a name = "gettingstarted"></a>

### Installation <a name = "installation"></a>

Running MSDScript from the CLI is the easiest way to use the program. You can also use it in your own program. This section will cover both use cases.

#### Installation for CLI use

Once you have downloaded MSDScript, we will use a tool called `CMake` to generate a `Makefile`. To generate the makefile, navigate to the msdscript folder and enter the command `cmake .` this will generate a `Makefile` specific to your system. Once you have the Makefile, run the command `make` to compile the the program. `make` will also generate a `libMSDLib.a` library file which you can use for linking into your own program.

You can now run MSDScript from the command line by entering `./MSDScript` followed by one of these commands:

- `--help` This will give you this same list of commands
- `--test` Will conduct the unit tests to ensure the program is functioning properly
- `--interp` Will start the interpreter which will wait for user input
- `--step` Is the recommended way to run the interpreter and will function the same as `--interp`
- `--print` Echo's the input to the CLI
- `--pretty-print` Will echo the input to the CLI but with formatting

<b>Note:</b> When entering input into the interpreter, it will not interpret until it sees an `EOF` character. It will be necessary to enter `ctrl-d` after entering your input for the interpreter to interpret the input.

#### Using MSDScript in your own program

Place the msdscript directory into your project root folder. You will then need to `#include "cmdline.h"` in your program. (Depending on your file structure, it may be necessary to include the relative path to `cmdline.h`. e.g. `#include "/msdscript/cmdline.h"`). You will then need to recompile your program and link to the msdscript library. You can create the library by following the instructions for installation on the command line. (The make file will create a library file which you can use to link). You are now able to call MSDScript functions from your own program.

Linking Example: `c++ -std=c++14 -o myProgram myProgram.cpp libMSDLib.a`

<b>Note:</b> You will need to compile with `-std=c++14` or higher in order for the program to compile correctly

<b> Note:</b> MSDScript can handle whitespace in expressions and will parse expressions with whitespace in them. e.g. `3     + 3` will correctly parse

The easiest way to use MSDScript in your program is to pass in a string to the `parse_str()` function. Note that `parse_str()` returns an `Expr` type which can then be passed to an interpret function like so:

<b>Note:</b> MSDScript also allows the use of parenthesis to group expressions and will handle them as expected. e.g. `(_let x = 3 _in x+1) + 3` will interpret as `7`

```
PTR(Expr) e = parse_str("2+4"); //turns your string into an Expression

PTR(Val) result = Step::interp_by_steps(e); // this will interpret your string and returns a `Val`

To see the value you can call the `to_string()` method of the `Val` class:

std::cout << result->to_string(); //prints out result to std::cout

```

<b>Note:</b> MSDScript implements a macro which makes the code more readable. The macro `PTR(Expr)` is really `std::shared_ptr<Expr>`

### Grammar <a name = "grammar"></a>
```
    〈expr〉=〈number〉
           |〈boolean〉
           |〈expr〉==〈expr〉
           |〈expr〉+〈expr〉
           |〈expr〉*〈expr〉
           |〈expr〉(〈expr〉)
           |〈variable〉
           | _let〈variable〉=〈expr〉_in〈expr〉
           | _if〈expr〉_then〈expr〉_else〈expr〉
           | _fun(〈variable〉)〈expr〉
```

This grammar shows the ways in which MSDScript interprets expressions `Expr`. All of the subclasses of Expression are: numbers, booleans, equality expressions, addition expressions, multiplication expressions, call expressions, variables, let expressions, if expressions and function expressions. You will find a description and examples of each of these below:

Numbers are any whole numbers (MSDScript does not support doubles or floats)

```
Correct usage: 1, 50, -4, 10000

Incorrect usage: 1.0, -4.0, 10_000, 10,000

```

Booleans are either `_true` or `_false`

```
Correct usage: _true, _false

Incorrect usage: true, True, TRUE

```

Equality Expressions compare two `expr` objects and return `_true` or `_false` You can compare Numbers or Expressions

```
3 == 3 will return _true
0 == 1 will return _false

(3+3) == (3+3) will return _true

(1+2) == 3 will return _true

```
Addition expressions are expressions with an expression on either side of the `+`

```

2+2

3 + 3

-4 + 5

```

Multiplication expressions are the same as addition expressions

```
2*2

3 * 3

-4 * 5

```

Call Expressions are used to call functions `_fun`. Because functions themselves are expressions, you can use a `_let` expression to create a function bound to a variable, for example, and then call it later. You can also create a function and call it right away with a call expression.

```
(_fun (x) x + 1)(10) this would output 11.

_let f = _fun (x) x+1
_in f(10)

```

Variables are alphabetical characters `a-z` and `A-Z` no special characters or spaces are allowed

```
Correct: a, b, z, temp, Temp

Incorrect: _c, a-b, #2, t3mp 

```

Let expressions can be used to assign values to variables and create complex functions. The key syntax for a let expression is `_let〈variable〉=〈expr〉_in〈expr〉` Any type of expression can be used in the expression field of a let expression including other let expressions, if expressions etc.

```
_let x = 3 _in x + 3

Result: 6

_let x = 3
_in  _if x == 3
     _then 1
     _else 0

Result: 1

```

<b>Note:</b> Let expression variables are considered bound after the `_in` statement and cannot be accessed until then.

If expressions are conditionals that can be used anywhere an expression is used. If expressions follow the pattern: `_if〈expr〉_then〈expr〉_else〈expr〉`

```

_if x == 2 _then _true _else _false

Result: _false

1 + (_if 3 == 3 _then 1 _else 0)

Result: 2

```

Function Expressions are functions which can be defined with the following syntax: `_fun(〈variable〉)〈expr〉`

```
_fun(x) x + 1

As shown in call expressions, functions can also be defined in a _let expression:

_let f = _fun (x) x-1
_in f(10)

Result: 9

You can build and call functions recursively to make more complex functions:

_let countdown = _fun(countdown)
                   _fun(n)
                     _if n == 0
                     _then 0
                     _else countdown(countdown)(n + -1)
_in countdown(countdown)(1000000)

```



