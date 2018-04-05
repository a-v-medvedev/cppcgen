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

#include "cppcgen_expr.h"
#include "cppcgen_exprdir.h"

namespace cppcgen {

class serial;

struct basic_expr {
    std::string str;
    expression *sexpr;
    basic_expr(std::string _str) : str(_str), sexpr(NULL) { 
        sexpr = new expression(str.c_str());
    }
    virtual std::string translate() const {
        std::string res = sexpr->translate(expression_directory::get_default_classes());
        return res;
    }
    virtual ~basic_expr() { delete sexpr; }
    operator serial() const;
};

}

