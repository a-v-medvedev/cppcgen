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

using namespace cppcgen;

void find_permutations(std::string str, std::vector<std::string> &result) {
    std::vector<std::string> parts;
    if (Helpers::Split(str, ",", parts) == 0)
        return;
    std::sort(parts.begin(), parts.end());
    do {
        std::string s;
        for (size_t i = 0; i < parts.size(); i++) {
            s += parts[i];
            if (i < parts.size() - 1)
                s += ",";
        }
        result.push_back(s);
    } while (std::next_permutation(parts.begin(), parts.end()));
}

void recursive_arrangements(std::vector<std::string> &arr, std::string str, 
                            std::vector<std::string> &result, size_t i, size_t r)
{
    if (i >= arr.size())
        return;
    if (str == "")
        str += arr[i];
    else
        str += "," + arr[i];
    if (r == 1) {
        result.push_back(str);
        return;
    }
    for(size_t j = 1; j < arr.size() - i + 1; j++)
        recursive_arrangements(arr, str, result, i + j, r - 1);
}

void find_arrangements(std::string str, std::vector<std::string> &result, int r)
{
    std::vector<std::string> parts;
    if (Helpers::Split(str, ",", parts) == 0)
        return;
    std::vector<std::string> tmp_result;
    std::string tmp;
    for (size_t i = 0; i < parts.size(); i++)
        recursive_arrangements(parts, tmp, tmp_result, i, r);
    for (size_t i = 0; i < tmp_result.size(); i++)
        find_permutations(tmp_result[i], result);
}

int main()
{
    typedef expression_directory dir;
    typedef std::pair<const std::string, const std::string> macro;

    output out;

    auto sum = dir::add_class("Sum");
    sum << macro { "args", "arg1,arg2,arg3,arg4" }
        << macro { "types", "int,float,double,std::string" }
        << macro { "RETURN_TYPE", "double" }
        << macro { "int_to_double", "(double)" }
        << macro { "float_to_double", "(double)" }
        << macro { "double_to_double", "" }
        << macro { "std::string_to_double", "atof" };
    dir::set_as_default(sum);
    
    {
        std::vector<std::string> permutations;
        find_permutations("int,float,double,std::string", permutations);
        for (size_t i = 0; i < permutations.size(); i++) {
            dir::add_macro("Sum", "types", permutations[i].c_str());
            out << 
               function_clause("{RETURN_TYPE}", 
                               "sum", 
                               "$EACH${{type} {arg} @ type={types};arg={args} @, }") (
                   return_clause()(basic_expr("({RETURN_TYPE})($EACH${{{type}_to_{RETURN_TYPE}}({arg}) @ type={types};arg={args} @ + })"))
               );
        }
    }

    {
        for (size_t j = 2; j < 5; j++) {
            std::vector<std::string> arrangements;
            find_arrangements("int,float,double,std::string", arrangements, j);
            sum << macro { "N", to_string(j) };
            sum << macro { "args", "$EACH${arg{I} @ I=$SEQ${1..{N}} @,}" };
            for (size_t i = 0; i < arrangements.size(); i++) {
                sum << macro { "types", arrangements[i].c_str() };
                out << 
                    function_clause("{RETURN_TYPE}", 
                                    "sum", 
                                    "$EACH${{type} {arg} @ type={types};arg={args} @, }") (
                        return_clause()(basic_expr("({RETURN_TYPE})($EACH${{{type}_to_{RETURN_TYPE}}({arg}) @ type={types};arg={args} @ + })"))
                    );
            }
        }
    }

    std::cout << out.get_str();
    return 0;
}
