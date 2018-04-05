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

using namespace cppcgen;

struct integer : public branch {
    int a;
    integer(int _a) : branch(), a(_a) {}
    virtual void print_prolog(output &out) const { out << "{ " << to_string(a) << " "; }
    virtual void print_epilog(output &out) const { out << "} "; }
};

int main()
{
    typedef expression_directory dir;
    typedef std::pair<const std::string, const std::string> macro;
    auto common = dir::add_class("Common");
    common << macro { "DIM", "2D" }
           << macro { "AAA", "BBB" };
    dir::set_as_default(common);

    output out;
    out << integer(5)(integer(4) << integer(314)) << integer(3) << integer(2) << basic_expr("EXPR") << 
               basic_expr("{AAA}->{DIM}");
    std::cout << out.get_str() << std::endl;
    return 0;
}
