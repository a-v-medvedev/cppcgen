/*
    Copyright (C) 2011-2013,2018 Alexey V. Medvedev

    This file is part of cppcgen. cppcgen is a generator of C code written in C++-11.

    cppcgen is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cppcgen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cppcgen.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "cppcgen_helpers.h"
#include "cppcgen_pair.h"
#include "cppcgen_expr.h"

namespace cppcgen {

struct eval_style_token;

struct expression {
    std::string str;
    std::string translate(std::string classes);
    bool parse_function(std::string name, std::string &str, char *t, 
            eval_style_token &token);
    std::string expand(const std::string str, std::string &classes);
    std::string find_option(std::string my_options, std::string opt);
    expression(std::string _str) : str(_str) {}
    expression() {}
};

}
