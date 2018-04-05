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

int main()
{
    using namespace cppcgen;
    typedef expression_directory dir;
    typedef std::pair<const std::string, const std::string> macro;
    
    auto common = dir::add_class("Common");
    common << macro { "DIM", "2D" }
           << macro { "AAA", "BBB" };
    dir::set_as_default(common);
    std::cout << basic_expr("{AAA}->{DIM}").translate() << std::endl;
    return 0;
}
