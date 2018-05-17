/*
    Copyright (C) 2011-2013,2018 Alexey V. Medvedev

    This file is part of cppcgen. cppcgen is a generator of C code written in C++-11.

    cppcgen is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cppcgen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cppcgen.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "cppcgen.h"
#include "cppcgen_utils.h"

using namespace cppcgen;

int main()
{
    auto common = dir::add_class("Common");
    common << macro { "arg1", "a" }
           << macro { "arg2", "b" }
           << macro { "func_name", "min" }
           << macro { "bin_operation", "<=" }
           << macro { "type", "int" };

    dir::set_as_default(common);
    {
        output out;
        out <<  function_("{type}", "{func_name}", "{type} {arg1}, {type} {arg2}")(
                    if_("{arg1} {bin_operation} {arg2}")(
                        return_()("{arg1}")
                    ) << else_()(
                        return_()("{arg2}")
                    )
                );
        std::cout << out.get_str();
    }
    {
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
                out <<  function_clause("{type}", "{func_name}", "{type} a, {type} b")(
                            if_clause("a {bin_operation} b")(
                                return_clause()("a")
                            ) << else_clause()(
                                return_clause()("b")
                            )
                );
            }
        }
        std::cout << out.get_str();
    }
    return 0;
}
