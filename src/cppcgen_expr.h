/*
    Copyright (C) 2011-2013,2018 Alexey V. Medvedev

    This file is part of cppcgen. cppcgen is a C++-11 generator of C code with macroses.

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

namespace cppcgen {

struct subst_style_token;
struct token;

struct subst {
    std::string from;
    std::string to;
    std::vector<subst> subordinate;
    subst(std::string aFrom, std::string aTo) : from(aFrom), to(aTo) {}
    subst(std::vector<subst> &aSub) : subordinate(aSub) {}
    subst() {}
    void act(std::string &Str);
    static void remove_syntax_helpers(std::string &final_string);
};

struct expr {
    std::string str;
    std::string render(std::vector<subst> &substs);
    static void fix_syntax(std::string &final_string);
    expr(std::string _str) : str(_str) {}
};

}
