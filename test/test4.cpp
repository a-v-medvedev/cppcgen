/*
    Copyright (C) 2011-2013,2018 Alexey V. Medvedev

    This file is part of cppcgen. cppcgen is a C++-11 generator of C code with macroses.

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

using namespace cppcgen;

int main()
{
    typedef expression_directory dir;
    typedef std::pair<const std::string, const std::string> macro;

    auto common = dir::add_class("Common");
        common << macro { "arg1", "a" }
               << macro { "arg2", "b" }
               << macro { "func_name", "min" }
               << macro { "bin_operation", "<=" }
               << macro { "type", "int" };

    dir::set_as_default(common);
    output out;
    out <<  function_clause("{type}",
                            "{func_name}",
                            "{type} {arg1}, {type} {arg2}")(
                if_clause("{arg1} {bin_operation} {arg2}")(
                    return_clause()(basic_expr("{arg1}"))
                ) << else_clause()(
                    return_clause()(basic_expr("{arg2}"))
                )
            );
    std::cout << out.get_str();
    return 0;
}
