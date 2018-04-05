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

#include <iostream>
#include <sstream>

#include "cppcgen_basic_expr.h"
#include "cppcgen_output.h"

namespace cppcgen {

struct term {
    const term *chained;
    term() : chained(NULL) {}
    virtual void print(output &out) const = 0;
};

struct serial : public term {
    const basic_expr *e;
    serial() : term(), e(NULL) {}
    explicit serial(const basic_expr &_e) : term(), e(&_e) {}
    virtual void print_self(output &out) const {
        if (e) { out << e->translate(); }
    }
    virtual void print(output &out) const final {
        print_self(out);
        if (chained) chained->print(out); 
    }
};

struct branch : public term {
    const term *nested;
    branch() : term(), nested(NULL) {}
    const term &operator()(const term &_nested) { nested = &_nested; return *this; }
    const term &operator()(const branch &_nested) { nested = &_nested; return *this; }
    const term &operator()(const serial &_nested) { nested = &_nested; return *this; }
    virtual void print_prolog(output &out) const = 0;
    virtual void print_epilog(output &out) const = 0;
    virtual void print(output &out) const final { 
        print_prolog(out); 
        if (nested) nested->print(out); 
        print_epilog(out); 
        if (chained) chained->print(out); 
    }
};

output &operator<<(output &out, const term &t);
output &operator<<(output &out, const serial &t);
output &operator<<(output &out, const std::string &s);

const term &operator<<(const term &lhs, const term &rhs);
const term &operator<<(const term &lhs, const serial &rhs);
const term &operator<<(const serial &lhs, const term &rhs);
const term &operator<<(const serial &lhs, const serial &rhs);

}
