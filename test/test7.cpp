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
#include <memory>
#include <assert.h>
#include "cppcgen.h"
#include "cppcgen_utils.h"

using namespace cppcgen;

std::vector<macro> generate_basic_macroses() {
    std::vector<macro> basic;
    basic += format_macroses("bnd_%s", "%s", { {"d0", "1"}, {"d1","D1"}, {"d2","D2"}, {"d3","D3"}, {"d4","D4"}});
    basic += format_macroses("%s_step", "$EACH${{bnd_{D}} @ D=%s @ * }", { {"d1","d0"}, {"d2","d0,d1"}, {"d3","d0,d1,d2"}, {"d4","d0,d1,d2,d3"}});

    basic += macro { "const_direction", "{it{NDIM}}" };
    basic += macro { "ALL_DIRECTIONS", "$EACH${d{N} @ N=$SEQ${1..{NDIM}} @,}" };
    basic += macro { "args", "$EACH${size_t {bnd_{D}} @ D={ALL_DIRECTIONS} @, }" };

    basic += macro { "idx_calc", "$EACH${{DIR} * {{DIR}_step} @ DIR={ALL_DIRECTIONS} @ + }" };
    return basic;
}

std::vector<std::vector<macro>> generate_boundary_macroses() {
    int ndim = macro_to_int("{NDIM}");
    std::vector<std::vector<macro>> combinations;
    switch (ndim) {
        case 1: combinations = std::vector<std::vector<macro> > {
                    { { "it1", "d1" } }
                };
                break;
        case 2: combinations = std::vector<std::vector<macro> > {
                    { { "it1", "d1" }, { "it2", "d2" } },
                    { { "it1", "d2" }, { "it2", "d1" } }
                };
                break;
        case 3: combinations = std::vector<std::vector<macro> > {
                    { { "it1", "d2" }, { "it2", "d1" }, { "it3", "d3" } },
                    { { "it1", "d3" }, { "it2", "d2" }, { "it3", "d1" } },
                    { { "it1", "d3" }, { "it2", "d1" }, { "it3", "d2" } }
                };
                break;
        default:
            FOR_EACH_IN_LIST(i, ndim, dirs, "{ALL_DIRECTIONS}") {
                auto dirs_without_const_dir = dirs;
                dirs_without_const_dir.erase(dirs_without_const_dir.begin() + i);
                std::vector<macro> comb;
                comb.push_back({ Format("it%d", ndim), dirs[i] });
                for (size_t j = 0; j < dirs_without_const_dir.size(); j++)
                    comb.push_back({ Format("it%d", ndim - 1 - j), dirs_without_const_dir[j] });
                combinations.push_back(comb);
            }
    }
    return combinations;
}

std::vector<std::vector<macro>> generate_front_rear_macroses()
{
    return std::vector<std::vector<macro>> {
        { { "const_coord_value", "{NCELLS}-1" }, { "sign", "1" } },
        { { "const_coord_value", "{bnd_{const_direction}}-{NCELLS}" }, { "sign", "-1" } }
    };
}

term &generate_nested_loops_operators() 
{
    int ndim = macro_to_int("{NDIM}");
    switch (ndim) {
        case 1: return !block_clause();
        case 2: return !for_("size_t {it1} = 0", "{it1} < {bnd_{it1}}", "{it1}++");
        case 3: return !for_("size_t {it1} = 0", "{it1} < {bnd_{it1}}", "{it1}++")(
                           for_("size_t {it2} = 0", "{it2} < {bnd_{it2}}", "{it2}++")
                       );
        case 4: return !for_("size_t {it1} = 0", "{it1} < {bnd_{it1}}", "{it1}++")(
                            for_("size_t {it2} = 0", "{it2} < {bnd_{it2}}", "{it2}++")(
                                for_("size_t {it3} = 0", "{it3} < {bnd_{it3}}", "{it3}++")
                           )  
                       );
 
        default: assert(false);
    }
}

term &generate_inloop_assignment_operators() {
    int ncells = macro_to_int("{NCELLS}");
    switch (ncells) {
        case 1:
            return !assign_("arr[idx]", "arr[idx + ({sign}) * ({{const_direction}_step})]");
        case 2:
            return !(assign_("arr[idx]", 
                             "arr[idx + ({sign}) * ({{const_direction}_step})]") 
                     << assign_("arr[idx - ({sign}) * ({{const_direction}_step})]", 
                                "arr[idx + ({sign}) * ({{const_direction}_step})]"));
        default:
            assert(false);
    }
}

int main()
{
    output out;

    out << "// Test it:  ./test7 > test7_autogen.cpp && g++ test7_stub.cpp test7_autogen.cpp && ./a.out\n\n";
    out << "#include <stdlib.h>\n\n";

    auto macroses = dir::add_class("Boundary");
    dir::set_as_default(macroses);    
    macroses << generate_basic_macroses();
     
    for (int ncells = 1; ncells <= 2; ncells++) {
        macroses << int_to_macro("NCELLS", ncells); 
        for (int ndim = 1; ndim <= 4; ndim++) {
            // Reset the Number Of Dimensions macro first
            macroses << int_to_macro("NDIM", ndim); 

            // Declare a i-dimensions update function clause
            function_ update("void", "update_boundary_{NCELLS}C_{NDIM}D", "double *arr, {args}");
            output function_body;
            update(
                function_body
            );

            // Now fill in the function_body with the apropriate contents

            // Update macroses to describe all the boundary kinds we have for i-dimensions case
            auto boundary_kinds = generate_boundary_macroses();

            // Update macroses which switch front and rear boundary of a given kind
            auto front_or_rear = generate_front_rear_macroses();

            // Get the operators for all the loops we have for each boundary
            term &all_loops = generate_nested_loops_operators();

            // Get all operators for the useful assignments to do inside a loop
            term &all_assignments = generate_inloop_assignment_operators();
          
            // Go through all the boundaries
            for (auto &boundary : boundary_kinds) {
                macroses << boundary;
                // Switch 'front' and 'rear' case for each boundary
                for (auto &f_or_r : front_or_rear) {
                    macroses << f_or_r;

                    // Put is all together
                    function_body <<          
                        all_loops(
                            assign_("size_t", "{const_direction}",  "{const_coord_value}") 
                            << assign_("size_t", "idx", "{idx_calc}") 
                            << all_assignments
                        );
                }
            }
            
            // Render the whole i-dimensions function into output.
            // We can't do this rendering later, since we change most macroses 
            // when go to the next dimension
            out << update;
        }
    }
    std::cout << out.get_str();
    return 0;
}
