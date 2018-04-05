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

#include <iostream>
#include "cppcgen_term.h"

namespace cppcgen {
output &operator<<(output &out, const term &t) { t.print(out); return out; }
output &operator<<(output &out, const serial &t) { t.print(out); return out; }
output &operator<<(output &out, const branch &t) { t.print(out); return out; }
output &operator<<(output &out, const std::string &s) {
    std::string str = s, spaces(out.level * 4, ' ');
    std::string replace = "\n" + spaces;
    for (std::string::size_type i = 0; (i = str.find("\n", i)) != std::string::npos;) {
        if (i >= str.length() - 1)
            break;
        str.replace(i, 2, replace);
        i += replace.length();
    }
    if (out.newline)
        out.s << spaces;
    out.s << str;
    out.newline = (str[str.length()-1] == '\n');
    return out; 
}
//output &operator<<(output &out, const std::ostream &os) { out.s << os; return out; }

const term &operator<<(const term &lhs, const term &rhs) { 
    const term *prev = &lhs; 
    for (const term *next = prev->chained; next; prev = next, next = next->chained); 
    const_cast<term *>(prev)->chained = &rhs; 
    return lhs; 
}

const term &operator<<(const term &lhs, const serial &rhs) { 
    return operator<<((const term &)lhs, (const term &)rhs); 
}

const term &operator<<(const serial &lhs, const term &rhs) { 
    return operator<<((const term &)lhs, (const term &)rhs); 
}

const term &operator<<(const serial &lhs, const serial &rhs) { 
    return operator<<((const term &)lhs, (const term &)rhs); 
}
}

