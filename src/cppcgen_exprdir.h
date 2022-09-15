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

#include <stdexcept>
#include "cppcgen_expression.h"
#include "cppcgen_pair.h"

namespace cppcgen {

template <class T>
struct map_helper {
    std::map<const std::string, T> m;
    bool add(const std::string &key, T val) {
        if (m.count(key))
            return false;
        m[std::string(key)] = val;
        return true;
    }
    bool find(const std::string &key, T &val) const {
        if (m.count(key) == 0)
            return false;
        val = (m.find(key))->second;
        return true;
    }
};

struct expression_class {
    const std::string name;
    expression_class(const std::string &_name) : name(_name) { }
    friend expression_class &operator <<(expression_class &cl, std::pair<const std::string, const std::string> macro);
    friend expression_class &operator <<(expression_class &cl, std::vector<std::pair<const std::string, const std::string> > macroses);
};

struct term;

struct expression_directory {
    struct saver {
        saver();
        static saver instance;
        static std::vector<term *> *storage;
        static void save(term *ptr);
        ~saver();
    };

    static map_helper<std::vector<pair<std::string> > *> directory;
    static map_helper<size_t> expr_dir;
    static std::vector<expression> expr_dir_array;
    static std::string defaults;  
    static expression_class add_class(std::string Name) {
        std::vector<pair<std::string> > *pairs = new std::vector<pair<std::string> >(); 
        if (directory.find(Name, pairs)) { std::runtime_error("ExprDirector::add_class: duplicate definition of a class"); }
        directory.add(Name, pairs);
        return expression_class(Name);
    }
    static void add_macro(std::string Name, std::string From, std::string To) {
        std::vector<pair<std::string> > *pairs;
        bool res = directory.find(Name, pairs);
        if (!res) { std::runtime_error("ExprDirector::add_macro: no such class"); }
        if (res) {
            for (size_t i = 0; i < pairs->size(); i++) {
                if ((*pairs)[i].first == From) {
                    (*pairs)[i].second = To;
                    return;
                }
            }
            pairs->push_back(pair<std::string>(From, To));
        }
    }
    static std::vector<subst> get_subst_set(std::string Name) {
        std::vector<pair<std::string> > *pairs;
        bool res = directory.find(Name, pairs);
        if (!res) { std::runtime_error("ExprDirector::get_subst_set: no such class"); }
        std::vector<subst> Result;
        if (res) {
            for (size_t i = 0; i < pairs->size(); i++) {
                Result.push_back(subst((*pairs)[i].first, (*pairs)[i].second));
            }
        }
        return Result;
    }
    static void add_expr(std::string Name, expression &Expr) {
        size_t Index = 0;
        bool res = expr_dir.find(Name, Index);
        if (res) {
            expr_dir_array[Index] = Expr;
        } else {
            expr_dir_array.push_back(Expr);
            Index = expr_dir_array.size() - 1;
            expr_dir.add(Name, Index);
        }
    }
    static expression &get_expr(std::string Name) {
        bool res;
        size_t Index = 0;
        res = expr_dir.find(Name, Index);
        if (!res) {
            throw std::runtime_error("ExprDirectory: Can't find the expression");
        }
        return expr_dir_array[Index];
    }
    static std::string get_default_classes() {
        return defaults;
    }
    static void set_default_classes(std::string _default) {
        defaults = _default;
    }

    static void set_as_default(expression_class &cl) { 
        std::string s;
        Format(s, "Class:%s", cl.name.c_str());
        set_default_classes(s);
    }
    static void add_to_defaults(expression_class &cl) { 
        std::string s = get_default_classes();
        Format(s, "%s Class:%s", s.c_str(), cl.name.c_str());
        set_default_classes(s);
    }
};

}
