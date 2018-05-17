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

void make_combinations(std::vector<std::vector<macro> > &combinations) {
    combinations.clear();
    int ndim = translate_to_int("{NDIM}");
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
}

term &make_nested_loops(int dim) 
{
    switch (dim) {
        case 1: return !block_clause();
        case 2: return !for_clause("size_t {it1} = {start_{it1}}", "{it1} < {stop_{it1}}", "{it1}++");
        case 3: return !for_clause("size_t {it1} = {start_{it1}}", "{it1} < {stop_{it1}}", "{it1}++")(
                           for_clause("size_t {it2} = {start_{it2}}", "{it2} < {stop_{it2}}", "{it2}++")
                       );
        case 4: return !for_clause("size_t {it1} = {start_{it1}}", "{it1} < {stop_{it1}}", "{it1}++")(
                            for_clause("size_t {it2} = {start_{it2}}", "{it2} < {stop_{it2}}", "{it2}++")(
                                for_clause("size_t {it3} = {start_{it3}}", "{it3} < {stop_{it3}}", "{it3}++")
                           )  
                       );
 
        default: assert(false);
    }
}

term &make_all_assignments(int bthick) {
    switch (bthick) {
        case 1:
            return !assignment_clause("arr[idx]", "arr[idx + ({sign}) * ({{const_direction}_step})]");
        case 2:
            return !(assignment_clause("arr[idx]", 
                                      "arr[idx + ({sign}) * ({{const_direction}_step})]") 
                     << assignment_clause("arr[idx - ({sign}) * ({{const_direction}_step})]", 
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

    int bthick = 1;  // single-cell boundary
//    int bthick = 2; // dual-cell boundary
    macroses << macro { "bnd_thickness", Format("%d", bthick) };
     
    macroses << format_macroses("start_%s", "%s", { {"d1","0"}, {"d2","0"}, {"d3","0"}, {"d4","0"}} ); 
    macroses << format_macroses("stop_%s", "({bnd_%s})", { {"d1","d1"}, {"d2","d2"}, {"d3","d3"}, {"d4","d4"}});
    macroses << format_macroses("bnd_%s", "%s", { {"d0", "1"}, {"d1","D1"}, {"d2","D2"}, {"d3","D3"}, {"d4","D4"}});
    macroses << format_macroses("%s_step", "$EACH${{bnd_{D}} @ D=%s @ * }", { {"d1","d0"}, {"d2","d0,d1"}, {"d3","d0,d1,d2"}, {"d4","d0,d1,d2,d3"}});

    macroses << macro { "const_direction", "{it{NDIM}}" };
    macroses << macro { "ALL_DIRECTIONS", "$EACH${d{N} @ N=$SEQ${1..{NDIM}} @,}" };
    macroses << macro { "args", "$EACH${size_t {bnd_{D}} @ D={ALL_DIRECTIONS} @, }" };

    macroses << macro { "idx_calc", "$EACH${{DIR} * {{DIR}_step} @ DIR={ALL_DIRECTIONS} @ + }" };

    std::vector<std::vector<macro> > front_or_rear =
    {
        { { "const_coord_value", "{bnd_thickness}-1" }, { "sign", "1" } },
        { { "const_coord_value", "{bnd_{const_direction}}-{bnd_thickness}" }, { "sign", "-1" } }
    };

    for (int ndim = 1; ndim <= 4; ndim++) {
        // Reset the Number Of Dimensions macro first
        macroses << macro { "NDIM", to_string(ndim) };

        // Declare a i-dimensions update function clause
        function_clause update("void", "update_boundary_{NDIM}D", "double *arr, {args}");
        output function_body;
        update(
            function_body
        );

        // Now fill in the function_body with the apropriate contents

        // Update macroses to describe all the boundaries we have for i-dimensions case
        std::vector<std::vector<macro> > combinations;
        make_combinations(combinations);

        // Create all the loops we need for each boundary
        term &all_loops = make_nested_loops(ndim);
       
        // Go through all the boundaries
        for (size_t i = 0; i < combinations.size(); i++) {
            macroses << combinations[i];
            // Switch 'front' and 'rear' case for each boundary
            for (size_t j = 0; j < front_or_rear.size(); j++) {
                macroses << front_or_rear[j];

                // Create all the assignments to do inside a loop
                term &all_assignments = make_all_assignments(bthick);

                // Put is all together
                function_body <<          
                    all_loops(
                        decl_assignment_clause("size_t", "{const_direction}",  "{const_coord_value}") 
                        << decl_assignment_clause("size_t", "idx", "{idx_calc}") 
                        << all_assignments
                    );
            }
        }
        
        // Render the whole i-dimensions function into output.
        // We can't do this rendering later, since we change most macroses 
        // when go to the next dimension
        out << update;
    }
    std::cout << out.get_str();
    return 0;
}
