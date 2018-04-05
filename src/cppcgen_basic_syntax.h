#pragma once

#include "cppcgen_term.h"
#include "cppcgen_basic_expr.h"

namespace cppcgen {

struct function_clause : public branch {
    std::string type, name, args;
    function_clause(std::string _type, std::string _name, std::string _args) : branch(),
                                                                               type(_type),
                                                                               name(_name),
                                                                               args(_args) {}
    virtual void print_prolog(output &out) const {
        out << basic_expr(type) << " ";
        out << basic_expr(name) << "(";
        out << basic_expr(args) << ") {" << "\n";
        out.level_up();
    }
    virtual void print_epilog(output &out) const {
        out.level_down();
        out << "}" << "\n\n";
    }
};

struct if_clause : public branch {
    std::string cond;
    if_clause(std::string _cond) : branch(), cond(_cond) { }
    virtual void print_prolog(output &out) const {
        out << "if (";
        out << basic_expr(cond) << ")";
        out << "{" << "\n";
        out.level_up();
    }
    virtual void print_epilog(output &out) const {
        out.level_down();
        out << "}" << "\n";
    }
};

struct else_clause : public branch {
    else_clause() : branch() { }
    virtual void print_prolog(output &out) const {
        out << "else ";
        out << "{" << "\n";
        out.level_up();
    }
    virtual void print_epilog(output &out) const {
        out.level_down();
        out << "}" << "\n";
    }
};

struct return_clause : public branch {
    return_clause() : branch() {}
    virtual void print_prolog(output &out) const { out << "return "; }
    virtual void print_epilog(output &out) const { out << ";\n"; }
};

struct assignment_clause : public branch {
    std::string lval, type;
    assignment_clause(std::string _lval, std::string _type) : branch(),
                                                              lval(_lval),
                                                              type(_type) {}
    virtual void print_prolog(output &out) const { out << basic_expr(lval) << " = "; }
    virtual void print_epilog(output &out) const { out << ";\n"; }
};

}
