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

using namespace cppcgen;

typedef expression_directory dir;
typedef std::pair<const std::string, const std::string> macro;

static inline std::vector<macro> set_macroses(const char *f, const char *t, 
                                              const std::vector<macro> &v)
{
    std::vector<macro> m;
    for (auto it = v.begin(); it != v.end(); it++) {
        m.push_back(macro { Format(f, it->first.c_str()), Format(t, it->second.c_str()) });
    }
    return m;
}

#define FOR_EACH_IN_LIST(ARR, EXPR, SIZE) std::vector<std::string> ARR; \
                                    Helpers::Split(basic_expr(EXPR).translate(), ',', ARR); \
                                    size_t SIZE = ARR.size(); \
                                    for (size_t i = 0; i < SIZE; i++) 

int translate_to_int(std::string expr)
{
    return from_string(basic_expr(expr).translate());
}

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
                    { { "it1", "d1" }, { "it2", "d2" }, { "it3", "d3" } },
                    { { "it1", "d2" }, { "it2", "d3" }, { "it3", "d1" } },
                    { { "it1", "d1" }, { "it2", "d3" }, { "it3", "d2" } }
                };
                break;
        default:
            FOR_EACH_IN_LIST(dirs, "{ALL_DIRECTIONS}", ndim) {
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

int main()
{
    output out;

    auto bound = dir::add_class("Boundary");
    dir::set_as_default(bound);    
 
    bound << set_macroses("start_%s", "%s", { {"d1","1"}, {"d2","1"}, {"d3","1"}, {"d4","1"}} ); 
    bound << set_macroses("stop_%s", "({bnd_%s}-1)", { {"d1","d1"}, {"d2","d2"}, {"d3","d3"}, {"d4","d4"}});
    bound << set_macroses("bnd_%s", "%s", { {"d0", "1"}, {"d1","D1"}, {"d2","D2"}, {"d3","D3"}, {"d4","D4"}});
    bound << set_macroses("%s_step", "$EACH${{bnd_{D}} @ D=%s @ * }", { {"d1","d0"}, {"d2","d0,d1"}, {"d3","d0,d1,d2"}, {"d4","d0,d1,d2,d3"}});

    bound << macro { "const_direction", "{it{NDIM}}" };
    bound << macro { "ALL_DIRECTIONS", "$EACH${d{N} @ N=$SEQ${1..{NDIM}} @,}" };
    bound << macro { "args", "$EACH${size_t {bnd_{D}} @ D={ALL_DIRECTIONS} @, }" };

    bound << macro { "idx_calc", "$EACH${{DIR} * {{DIR}_step} @ DIR={ALL_DIRECTIONS} @ + }" };
    bound << macro { "new_value", "arr[idx + ({sign}) * ({{const_direction}_step})]" };

    std::vector<std::vector<macro> > front_or_rear =
    {
        { { "const_coord_value", "0" }, { "sign", "1" } },
        { { "const_coord_value", "{bnd_{const_direction}}" }, { "sign", "-1" } }
    };

    std::vector<std::vector<macro> > combinations;
  

    for (int ndim = 1; ndim < 5; ndim++) {
        bound << macro { "NDIM", to_string(ndim) };
        make_combinations(combinations);
        term &all_loops = make_nested_loops(ndim);
        function_clause upd("void", "update_boundary_{NDIM}D", "double *arr, {args}");
        upd.print_prolog(out);
        for (size_t i = 0; i < combinations.size(); i++) {
            bound << combinations[i];
            for (size_t j = 0; j < front_or_rear.size(); j++) {
                bound << front_or_rear[j];
                out <<          
                    all_loops(
                        decl_assignment_clause("size_t", "{const_direction}",  "{const_coord_value}") 
                        << decl_assignment_clause("size_t", "idx", "{idx_calc}") 
                        << assignment_clause("arr[idx]", "{new_value}")
                    );
            }
        }
        upd.print_epilog(out);
    }
    std::cout << out.get_str();
    return 0;
}
