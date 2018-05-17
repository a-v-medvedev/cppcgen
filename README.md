# cppcgen

**cppcgen** is a generator of C code written in C++-11.

Copyright (c) 2011-2013,2018 Alexey V. Medvedev

## License
The project is licensed under GNU Lesser Public License version 3.0.
See files COPYING and COPYING.LESSER for details.

## What is cppcgen?
**cppcgen** is a library which makes an attempt to allow for easy structured 
generation of C or simple C++ subroutines. It might be useful for automated 
generation of some C code portions which incorporate some repeated code 
structures or repeated similar complex expressions. 

The expressions definitions can use macroses, which are predefined at 
runtime in Expressions Directory. Besides the strightforward macroses expansion, 
the macroprocessor also recognizes and interpretes 
3 macro 'functions': `EVAL`, `EACH`, and `SEQ`.

`EVAL` expands a specific macro in an expression with a specific value 
given directly in the input string (not predefined in an Expression
Directory) and substitutes the result of expansion.

`EACH` simply shortcuts several `EVAL` calls in a row addressing the
comma-separated list of values to substitute each time.

`SEQ` function generates at runtime a simple comma-separated sequence 
of integer numbers.

## Examples

#### 1. Simple macro expansion:
```
auto common = dir::add_class("Common");
common << macro { "foo", "bar" };
dir::set_as_default(common);
std::cout << basic_expr("{foo}").translate() << std::endl;
```
Output:
```
> bar
```

#### 2. Nested macroses:
```
auto common = dir::add_class("Common");
common << macro { "foo1", "bar1" }
       << macro { "foo2", "bar2" }
       << macro { "N", "2" };
dir::set_as_default(common);
std::cout << basic_expr("{foo{N}}").translate() << std::endl;
```
Output:
```
> bar2
```

#### 3. Simple EVAL usage:
```
auto common = dir::add_class("Common");
common << macro { "foo1", "bar1" }
       << macro { "foo2", "bar2" };
dir::set_as_default(common);
std::cout << basic_expr("$EVAL${{foo{N}} @ N=1}").translate() << std::endl;
```
Output:
```
> bar1
```

#### 4. Simple EACH usage:
```
auto common = dir::add_class("Common");
common << macro { "foo1", "bar1" }
       << macro { "foo2", "bar2" };
dir::set_as_default(common);
std::cout << basic_expr("$EACH${{foo{N}} @ N=1,2 @ + }").translate() << std::endl;
```
Output:
```
> bar1 + bar2
```

#### 5. Simple SEQ usage:
```
auto common = dir::add_class("Common");
common << macro { "foo1", "bar1" }
       << macro { "foo2", "bar2" }
       << macro { "NUMBERS", "$SEQ${1..2}" };
dir::set_as_default(common);
std::cout << basic_expr("$EACH${{foo{N}} @ N={NUMBERS} @ * }").translate() << std::endl;
```
Output:
```
> bar1 * bar2
```
#### 6. Simple generation of C language operators, functions and other syntax elements:
```
auto common = dir::add_class("Common");
dir::set_as_default(common);
output out;
out <<  function_("int", "min", "int a, int b")(
            if_("a <= b")( 
                return_("a")
            ) << else_()(
                return_("b")
            )
        );
std::cout << out.get_str();
```
Output:
```
> int min(int a, int b) {
>     if (a <= b) {
>         return a;
>     }
>     else {
>         return b;
>     }  
> }
```
#### 7. C language operators generation combined with macro expansion:
```
auto common = dir::add_class("Common");
    common << macro { "arg1", "a" }
           << macro { "arg2", "b" }
           << macro { "func_name", "min" }
           << macro { "bin_operation", "<=" }
           << macro { "type", "int" };

dir::set_as_default(common);
output out;
out <<  function_("{type}", "{func_name}", "{type} {arg1}, {type} {arg2}")(
            if_("{arg1} {bin_operation} {arg2}")(
                return_("{arg1}")
            ) << else_()(
                return_("{arg2}")
            )
        );
std::cout << out.get_str();
```
Output:
```
> int min(int a, int b) {
>     if (a <= b) {
>         return a;
>     }
>     else {
>         return b;
>     }
> }
```

#### 8. Extension of previous case for min/max and int/float variation in generated code:

```
    auto common = dir::add_class("Common");
    dir::set_as_default(common);
    output out;

    std::vector<std::vector<macro>> operators = {
        { {"func_name", "max"}, {"bin_operation", ">="} },
        { {"func_name", "min"}, {"bin_operation", "<="} }
    };

    std::vector<macro> types = {
        { "type", "int" },
        { "type", "float" }
    };

    for (auto op : operators) {
        common << op;
        for (auto t : types) {
            common << t;
            out <<  function_("{type}", "{func_name}", "{type} a, {type} b")(
                        if_("a {bin_operation} b")(
                            return_("a")
                        ) << else_()(
                            return_("b")
                        )
            );
        }
    }
```
Output:
```
> int max(int a, int b) {
>     if (a >= b){
>         return a;
>     }
>     else {
>         return b;
>     }
> }
>
> float max(float a, float b) {
>     if (a >= b){
>         return a;
>     }
>     else {
>         return b;
>     }
> }
>
> int min(int a, int b) {
>     if (a <= b){
>         return a;
>     }
>     else {
>         return b;
>     }
> }
>
> float min(float a, float b) {
>     if (a <= b){
>         return a;
>     }
>     else {
>         return b;
>     }
> }
```

The `test/` subirectory of this projects may give a basic insight
of the library usage.

The most advanced test7.cpp implements a popular pattern of a boundary update for
1D, 2D, 3D and 4D regular grids for single-cell and dual-cell boundaries. 
You can test if the generated code works well with a line similar to this one:
```
./test7 > test7_autogen.cpp && g++ test7_stub.cpp test7_autogen.cpp && ./a.out
```


## Build instructions

GNU makefile was tested with `GNU make 4.0` and `GCC C++ compiler 4.7.4` only.
There is probaby some dependency on `GNU bash`, `GNU fileutils` and `GNU findutils` 
in some of Makefile commands.

The portability is not a strong goal for now, but code and Makefiles were 
made portable at the best effort.

Run:
```
> make
```
which is the same as:
```
> make lib distr tests
```
This will result in ready to re-use library and headers in `distr/` subdirectory
and some tests built and ready to run in `sandbox/` subdirectory.


## Tests

Run tests with the commands like:
```
> cd sandbox && ./test1 && cd ..
> cd sandbox && ./test2 && cd ..
...
```
Testing coverage is not elaborated yet. Let's hope the tests cover the functionality well.


