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
#include "cppcgen_utils.h"

using namespace cppcgen;

int main()
{
    typedef expression_directory dir;
    typedef std::pair<const std::string, const std::string> macro;

    output out;

    auto bound = dir::add_class("Boundary");
    bound << macro { "start_x", "1" } 
          << macro { "start_y", "1" } 
          << macro { "start_z", "1" }
          << macro { "stop_x", "({bnd_x}-1)" } 
          << macro { "stop_y", "({bnd_y}-1)" } 
          << macro { "stop_z", "({bnd_z}-1)" };

    bound << macro { "bnd_x", "X" } 
          << macro { "bnd_y", "Y" } 
          << macro { "bnd_z", "Z" };

    bound << macro { "z_step", "{bnd_x} * {bnd_y}" } 
          << macro { "y_step", "{bnd_x}" } 
          << macro { "x_step", "1" };

    bound << macro { "const_direction", "{it{NDIM}}" };

    auto b3d = dir::add_class("3D");
    b3d << macro { "NDIM", "3" };
    b3d << macro { "idx_calc", "z * {z_step} + y * {y_step} + x * {x_step}" };
    b3d << macro { "args", "size_t {bnd_x}, size_t {bnd_y}, size_t {bnd_z}" };

    auto b2d = dir::add_class("2D");
    b2d << macro { "NDIM", "2" };
    b2d << macro { "idx_calc", "y * {y_step} + x * {x_step}" };
    b2d << macro { "args", "size_t {bnd_x}, size_t {bnd_y}" };

    std::vector<std::vector<macro> > consts =
    {
        { { "const_value", "0" }, { "sign", "1" } },
        { { "const_value", "{bnd_{const_direction}}" }, { "sign", "-1" } }
    };

    std::vector<std::vector<macro> > planes =
    {
        { { "it1", "x" }, { "it2", "y" }, { "it3", "z" } },
        { { "it1", "y" }, { "it2", "z" }, { "it3", "x" } },
        { { "it1", "x" }, { "it2", "z" }, { "it3", "y" } }
    };

    std::vector<std::vector<macro> > lines =
    {
        { { "it1", "x" }, { "it2", "y" } },
        { { "it1", "y" }, { "it2", "x" } },
    };

  
    dir::set_as_default(bound);    
    dir::add_to_defaults(b3d);
    output function_body_3d;
    for (size_t i = 0; i < planes.size(); i++) {
        bound << planes[i];
        for (size_t j = 0; j < consts.size(); j++) {
            bound << consts[j];
            function_body_3d <<
                for_("size_t {it1} = {start_{it1}}", "{it1} < {stop_{it1}}", "{it1}++")(
                    for_("size_t {it2} = {start_{it2}}", "{it2} < {stop_{it2}}", "{it2}++")(
                        assign_("size_t", "{const_direction}",  "{const_value}") 
                        << assign_("size_t", "idx", "{idx_calc}") 
                        << assign_("arr[idx]",
                                   "arr[idx + ({sign}) * ({{const_direction}_step})]")
                    )
                );
        }
    }
    out << function_("void", "update_boundary_{NDIM}D", "double *arr, {args}")(
               function_body_3d
           );



    dir::set_as_default(bound);    
    dir::add_to_defaults(b2d);
    output function_body_2d;
    for (size_t i = 0; i < lines.size(); i++) {
        bound << lines[i];
        for (size_t j = 0; j < consts.size(); j++) {
            bound << consts[j];
            function_body_2d <<
                for_("size_t {it1} = {start_{it1}}", "{it1} < {stop_{it1}}", "{it1}++")(
                    assign_("size_t", "{const_direction}",  "{const_value}") 
                    << assign_("size_t", "idx", "{idx_calc}") 
                    << assign_("arr[idx]",
                               "arr[idx + ({sign}) * ({{const_direction}_step})]")
                );
            }
    }
    out << function_("void", "update_boundary_{NDIM}D", "double *arr, {args}")(
               function_body_2d
           );

    std::cout << out.get_str();
    return 0;
}
