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

struct boundary_update_definitions { 
    virtual std::vector<macro> generate_basic_macroses() {
        std::vector<macro> basic;
        basic += format_macroses("max_%s", "%s", { {"d0", "1"}, {"d1","D1"}, {"d2","D2"}, {"d3","D3"}, {"d4","D4"}});
        basic += format_macroses("%s_step", "$EACH${{max_{D}} @ D=%s @ * }", { {"d1","d0"}, {"d2","d0,d1"}, {"d3","d0,d1,d2"}, {"d4","d0,d1,d2,d3"}});

        basic += macro { "const_direction", "{it{NDIM}}" };
        basic += macro { "ALL_DIRECTIONS", "$EACH${d{N} @ N=$SEQ${1..{NDIM}} @,}" };
        basic += macro { "args", "$EACH${size_t {max_{D}} @ D={ALL_DIRECTIONS} @, }" };

        basic += macro { "idx_calc", "$EACH${{DIR} * {{DIR}_step} @ DIR={ALL_DIRECTIONS} @ + }" };
        basic += macro { "offset", "(({sign}) * ({{const_direction}_step}))" };
        return basic;
    }
    virtual std::vector<std::vector<macro>> generate_boundary_macroses() {
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
    virtual std::vector<std::vector<macro>> generate_front_rear_macroses()
    {
        return std::vector<std::vector<macro>> {
            { { "const_coord_value", "{NCELLS}-1" }, { "sign", "1" } },
            { { "const_coord_value", "{max_{const_direction}}-{NCELLS}" }, { "sign", "-1" } }
        };
    }
    virtual term &generate_nested_loops_operators() { return !block_clause(); } 
    virtual term &generate_inloop_assignment_operators() { return !block_clause(); }
};

struct CPU_NDIM_loops : virtual public boundary_update_definitions {
    virtual term &generate_nested_loops_operators() 
    {
        int ndim = macro_to_int("{NDIM}");
        switch (ndim) {
            case 1: return !block_clause();
            case 2: return !for_("size_t {it1} = 0", "{it1} < {max_{it1}}", "{it1}++");
            case 3: return !for_("size_t {it1} = 0", "{it1} < {max_{it1}}", "{it1}++")(
                               for_("size_t {it2} = 0", "{it2} < {max_{it2}}", "{it2}++")
                           );
            case 4: return !for_("size_t {it1} = 0", "{it1} < {max_{it1}}", "{it1}++")(
                                for_("size_t {it2} = 0", "{it2} < {max_{it2}}", "{it2}++")(
                                    for_("size_t {it3} = 0", "{it3} < {max_{it3}}", "{it3}++")
                                )  
                           );
     
            default: assert(false);
        }
    }
};

struct COPY : virtual public boundary_update_definitions {
    term &generate_inloop_assignment_operators() {
        int ncells = macro_to_int("{NCELLS}");
        switch (ncells) {
            case 1: return !assign_("arr[idx]", "arr[idx + {offset}]");
            case 2: return !(assign_("arr[idx]", "arr[idx + {offset}]") 
                             << assign_("arr[idx - {offset}]", "arr[idx + {offset}]"));
            default: assert(false);
        }
    }
};

struct LINEAR : virtual public boundary_update_definitions {
    term &generate_inloop_assignment_operators() {
        int ncells = macro_to_int("{NCELLS}");
        switch (ncells) {
            case 1: return !assign_("arr[idx]", "({f1}-{f0})+{f1}");
            case 2: return !(assign_("arr[idx]", "({f1}-{f0})+{f1}") 
                             << assign_("arr[idx - {offset}]", "({f1}-{f0})*2+{f1}"));
            default: assert(false);
        }
    }
    virtual std::vector<macro> generate_basic_macroses() {
        auto basic = boundary_update_definitions::generate_basic_macroses();
        basic += macro { "f0", "arr[idx + {offset}*2]" };
        basic += macro { "f1", "arr[idx + {offset}]" };
        return basic;
    }
};

struct NEWT2 : virtual public boundary_update_definitions {
    term &generate_inloop_assignment_operators() {
        int ncells = macro_to_int("{NCELLS}");
        switch (ncells) {
            case 1: return !assign_("arr[idx]", "{f0} + 3 * ({f1} - {f0}) + 3 * 2 * (({f2} - {f1}) - ({f1} - {f0})) / 2");
            case 2: return !(assign_("arr[idx]", "{f0} + 3 * ({f1} - {f0}) + 3 * 2 * (({f2} - {f1}) - ({f1} - {f0})) / 2") 
                             << assign_("arr[idx - {offset}]", "{f0} + 4 * ({f1} - {f0}) + 4 * 3 * (({f2} - {f1}) - ({f1} - {f0})) / 2"));
            default: assert(false);
        }
    }
    virtual std::vector<macro> generate_basic_macroses() {
        auto basic = boundary_update_definitions::generate_basic_macroses();
        basic += macro { "f0", "arr[idx + {offset}*3]" };
        basic += macro { "f1", "arr[idx + {offset}*2]" };
        basic += macro { "f2", "arr[idx + {offset}]" };
        return basic;
    }
};

struct COPY_CPU : public CPU_NDIM_loops, public COPY {
};

struct LINEAR_CPU : public CPU_NDIM_loops, public LINEAR {
};

struct NEWT2_CPU : public CPU_NDIM_loops, public NEWT2 {
};

std::shared_ptr<boundary_update_definitions> create_boundary_update_definitions(int order)
{
    switch (order) {
        case 0: return std::make_shared<COPY_CPU>();
        case 1: return std::make_shared<LINEAR_CPU>();
        case 2: return std::make_shared<NEWT2_CPU>();
        default: assert(false);
    }
}

int main()
{
    output out;

    out << "// Test it:  ./test7 > test7_autogen.cpp && g++ test7_stub.cpp test7_autogen.cpp && ./a.out\n\n";
    out << "#include <stdlib.h>\n\n";

    auto macroses = dir::add_class("Boundary");
    dir::set_as_default(macroses);

    // Loop over orders of approximation on boundary update (zero (copy), linear, 2nd order Newton's)
    for (int norder = 0; norder <= 2; norder++) {
        auto defs = create_boundary_update_definitions(norder);
        macroses << int_to_macro("NORDER", norder);
        // Get some basic macroses 
        macroses << defs->generate_basic_macroses();
        // Loop over amount of boundary cells
        for (int ncells = 1; ncells <= 2; ncells++) {
            macroses << int_to_macro("NCELLS", ncells);
            // Loop over grid dimensions  
            for (int ndim = 1; ndim <= 4; ndim++) {
                // Reset the Number Of Dimensions macro first
                macroses << int_to_macro("NDIM", ndim); 

                // Declare a i-dimensions update function clause
                function_ update("void", "update_boundary_{NCELLS}C_{NORDER}O_{NDIM}D", "double *arr, {args}");
                output function_body;
                update(
                    function_body
                );

                // Now fill in the function_body with the apropriate contents

                // Update macroses to describe all the boundary kinds we have for i-dimensions case
                auto boundary_kinds = defs->generate_boundary_macroses();

                // Update macroses which switch front and rear boundary of a given kind
                auto front_or_rear = defs->generate_front_rear_macroses();

                // Get the operators for all the loops we have for each boundary
                term &all_loops = defs->generate_nested_loops_operators();

                // Get all operators for the actual assignments to do inside a loop
                term &all_assignments = defs->generate_inloop_assignment_operators();
              
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
    }
    std::cout << out.get_str();
    return 0;
}
