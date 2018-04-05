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

 
#include "cppcgen_exprdir.h"

namespace cppcgen {

map_helper<std::vector<pair<std::string> > *> expression_directory::directory; 
map_helper<size_t> expression_directory::expr_dir;
std::vector<expression> expression_directory::expr_dir_array;
std::string expression_directory::defaults;

expression_class &operator <<(expression_class &cl, std::pair<const std::string, const std::string> macro) {
    expression_directory::add_macro(cl.name, macro.first, macro.second);
    return cl;
}

}
