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

namespace cppcgen {

class term;
class serial;
class branch;

class output {
    friend output &operator<<(output &out, const term &t);
    friend output &operator<<(output &out, const serial &t);
    friend output &operator<<(output &out, const branch &t);
    friend output &operator<<(output &out, const std::string &s);
    friend output &operator<<(output &out, const std::ostream &os);
    
    private:
    std::stringstream s;
    int level;
    bool newline;

    public:
    output() : level(0), newline(false) {}
    std::string get_str() const { return s.str(); }
    void level_up() { level++; }
    void level_down() { level--; }
};

}
