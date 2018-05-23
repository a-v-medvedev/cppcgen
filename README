# cppcgen

**cppcgen** is a generator of C code written in C++11. It is a library which allows C++11 programs to create C-subroutines as an output.

Copyright (c) 2011-2013,2018 Alexey V. Medvedev

## License
The project is licensed under GNU Lesser Public License version 3.0.
See files COPYING and COPYING.LESSER for details.

## What is cppcgen?
**cppcgen** is a library which makes an attempt to allow for easy structured 
generation of C or simple C++ subroutines. It might be useful for automated 
generation of some C code portions which incorporate some repeated code 
structures or repeated similar complex expressions. 

In simple cases it does what C++ templates do, but you can go much further with all the power
of C++ data structures, inheritance and other ways to express complex ideas.
For example, one can rather easily make three versions of some subroutine:
1) simple CPU version, 2) OpenMP version, 3) CUDA version. The main code
of subroutine will be the same, but you can't do the generalisation of these 3 versions 
with C++ templates because OpenMP version requires adding pragmas, and CUDA version
requires adding non-standard keywords and some changes in loops an index calculations.

The **ccpcgen** was designed to generalise some compute-intensive
subroutines in various versions which is a typical task for HPC. 
But the abilities of the generator are not tied to any programming field and 
may be also used in any kind of programming tasks. 

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

## test7.cpp
The most advanced test7.cpp implements a popular pattern of a boundary update for
1D, 2D, 3D and 4D regular grids.
You can test if the generated code works well with a line similar to this one:
```
./test7 > test7_autogen.cpp && g++ test7_stub.cpp test7_autogen.cpp && ./a.out
```

For 1D case, the boundary update problem looks like this. Suppose we have an array of N
numbers. We have to update the first and the last element of the array so that
they were equal to their neighbours.
```
int a[N];

{0} [1] [2] ... [N-2] {N-1} 

{0}<-[1] [2] ... [N-2]->{N-1} 
```
This can be done with just 2 operations:
```
a[0] = a[1];
a[N-1] = a[N-2];
```
The update function may look like this:
```
void update_1D(int *a, size_t N)
{
    a[0] = a[1];
    a[N-1] = a[N-2];
}
```
Then we may extend this definition for "two-cell boundary". Let's update two first and two last array elements:
```
int a[N];

{0} {1} [2] ... [N-3] {N-2} {N-1} 

{0}<-{1}<-[2] ... [N-3]->{N-2}->{N-1} 
```
There appear 2 more operations:
```
a[0] = a[2];
a[1] = a[2];
a[N-1] = a[N-3];
a[N-2] = a[N-3];
```
The separate update function may look like this:
```
void update_1D_2C(int *a, size_t N)
{
    a[0] = a[2];
    a[1] = a[2];
    a[N-1] = a[N-3];
    a[N-2] = a[N-3];
}
```

Now we extend the definition for 2D, 3D and 4D case and want to have different update methods: not only copy data from neighbours, but also implement some extrapolation as an option. This leads us to some bunch of update functions:

```
void update_boundary_1C_0O_1D(double *arr, size_t D1);
void update_boundary_1C_0O_2D(double *arr, size_t D1, size_t D2);
void update_boundary_1C_0O_3D(double *arr, size_t D1, size_t D2, size_t D3);
void update_boundary_1C_0O_4D(double *arr, size_t D1, size_t D2, size_t D3, size_t D4);
void update_boundary_2C_0O_1D(double *arr, size_t D1);
void update_boundary_2C_0O_2D(double *arr, size_t D1, size_t D2);
void update_boundary_2C_0O_3D(double *arr, size_t D1, size_t D2, size_t D3);
void update_boundary_2C_0O_4D(double *arr, size_t D1, size_t D2, size_t D3, size_t D4);
void update_boundary_1C_1O_1D(double *arr, size_t D1);
void update_boundary_1C_1O_2D(double *arr, size_t D1, size_t D2);
void update_boundary_1C_1O_3D(double *arr, size_t D1, size_t D2, size_t D3);
void update_boundary_1C_1O_4D(double *arr, size_t D1, size_t D2, size_t D3, size_t D4);
void update_boundary_2C_1O_1D(double *arr, size_t D1);
void update_boundary_2C_1O_2D(double *arr, size_t D1, size_t D2);
void update_boundary_2C_1O_3D(double *arr, size_t D1, size_t D2, size_t D3);
void update_boundary_2C_1O_4D(double *arr, size_t D1, size_t D2, size_t D3, size_t D4);
void update_boundary_1C_2O_1D(double *arr, size_t D1);
void update_boundary_1C_2O_2D(double *arr, size_t D1, size_t D2);
void update_boundary_1C_2O_3D(double *arr, size_t D1, size_t D2, size_t D3);
void update_boundary_1C_2O_4D(double *arr, size_t D1, size_t D2, size_t D3, size_t D4);
void update_boundary_2C_2O_1D(double *arr, size_t D1);
void update_boundary_2C_2O_2D(double *arr, size_t D1, size_t D2);
void update_boundary_2C_2O_3D(double *arr, size_t D1, size_t D2, size_t D3);
void update_boundary_2C_2O_4D(double *arr, size_t D1, size_t D2, size_t D3, size_t D4);
```
where "1C" and "2C" distinguishes versions for single-cell and dual-cell boundaries; "1D", "2D", "3D" and "4D" declares the grid dimensions; "0O", "1O" and "2O" defines the order of extrapolation polynomial used in the update function (0-order means just a direct copy of neighbour value as in the explanation above, 1st order means linear extrapolation with 2 neighbour points, 2nd order means the extrapolation with 2nd order Newton's polynomial using 3 neighbour points).

As an example, a hand-written code for 2D case with 2nd order extrapolation and dual-cell boundary may look like this:
```
void update_boundary_2C_2O_2D(double *arr, size_t X, size_t Y) {
    for (size_t x = 0; x < X; x++) {
        size_t idx = x + X;
        arr[idx] = arr[idx + X*3] + 3*(arr[idx + X*2] - arr[idx + X*3]) + 6*((arr[idx + X] - arr[idx + X*2]) - (arr[idx + X*2] - arr[idx + X*3]))/2;
        arr[idx - X] = arr[idx + X*3] + 4*(arr[idx + X*2] - arr[idx + X*3]) + 12*((arr[idx + X] - arr[idx + X*2]) - (arr[idx + X*2] - arr[idx + X*3]))/2;
    }
    for (size_t x = 0; x < X; x++) {
        size_t idx = x +  (Y-2) * X;
        arr[idx] = arr[idx - X*3] + 3*(arr[idx - X*2] - arr[idx - X*3]) + 6*((arr[idx - X] - arr[idx - X*2]) - (arr[idx - X*2] - arr[idx - X*3]))/2;
        arr[idx + X] = arr[idx - X*3] + 4*(arr[idx - X*2] - arr[idx - X*3]) + 7*((arr[idx - X] - arr[idx - X*2]) - (arr[idx - X*2] - arr[idx - X*3])) / 2;
    }
    for (size_t y = 0; y < Y; y++) {
        size_t idx = 1 + y * X;
        arr[idx] = arr[idx + 3] + 3*(arr[idx + 2] - arr[idx + 3]) + 6*((arr[idx + 1] - arr[idx + 2]) - (arr[idx + 2] - arr[idx + 3]))/2;
        arr[idx - 1] = arr[idx + 3] + 4*(arr[idx + 2] - arr[idx + 3]) + 12*((arr[idx + 1] - arr[idx + 2]) - (arr[idx + 2] - arr[idx + 3]))/2;
    }
    for (size_t y = 0; y < Y; y++) {
        size_t idx = (X-2) + y * X;
        arr[idx] = arr[idx - 3] + 3*(arr[idx - 2] - arr[idx - 3]) + 6*((arr[idx - 1] - arr[idx - 2]) - (arr[idx - 2] - arr[idx - 3]))/2;
        arr[idx + 1] = arr[idx - 3] + 4*(arr[idx - 2] - arr[idx - 3]) + 12*((arr[idx - 1] - arr[idx - 2]) - (arr[idx - 2] - arr[idx - 3]))/2;
    }
}
```

The test7.cpp makes an attempt of neat and structured generation of all these variations of update functions so that there is no runtime if's etc and
the generated code is as close to hand-written as possible when is compiled with any optimizing C compiler.

Another requirement is the generation of the CUDA version for all these functions. This task can be done rather easily, but the CUDA code generation is not implemented yet in this example, will be done in future.   

With the line:
```
./test7 > test7_autogen.cpp && g++ test7_stub.cpp test7_autogen.cpp && ./a.out
```
you can run a visual test of 2D versions of these update functions.

## Build instructions

GNU makefile was tested with `GNU make 4.0` and `GCC C++ compilers 4.7-6.0` only.
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


