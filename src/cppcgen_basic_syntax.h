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
    CLONE(function_clause)
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
    CLONE(if_clause)
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
    CLONE(else_clause)
};

struct block_clause : public branch {
    block_clause() : branch() { }
    virtual void print_prolog(output &out) const {
        out << "{" << "\n";
        out.level_up();
    }
    virtual void print_epilog(output &out) const {
        out.level_down();
        out << "}" << "\n";
    }
    CLONE(block_clause)
};

struct for_clause : public branch {
    std::string e1, e2, e3;
    for_clause(std::string _e1, std::string _e2, std::string _e3) : branch(),
                                                                         e1(_e1),
                                                                         e2(_e2),
                                                                         e3(_e3) {}
    virtual void print_prolog(output &out) const {
        out << "for (" << basic_expr(e1) << "; ";
        out << basic_expr(e2) << "; ";
        out << basic_expr(e3) << ") {" << "\n";
        out.level_up();
    }
    virtual void print_epilog(output &out) const {
        out.level_down();
        out << "}" << "\n";
    }
    CLONE(for_clause)
};

struct return_clause : public branch {
    return_clause() : branch() {}
    virtual void print_prolog(output &out) const { out << "return "; }
    virtual void print_epilog(output &out) const { out << ";\n"; }
    CLONE(return_clause)
};

struct assignment_clause : public serial {
    std::string lval, rval;
    assignment_clause(std::string _lval, std::string _rval) : serial(),
                                                              lval(_lval),
                                                              rval(_rval) {}
    virtual void print_self(output &out) const { 
        out << basic_expr(lval) << " = " << basic_expr(rval) << ";\n";
    }
    CLONE(assignment_clause)
};

struct decl_assignment_clause : public serial {
    std::string type, lval, rval;
    decl_assignment_clause(std::string _type, std::string _lval, std::string _rval) :
                                                              serial(),
                                                              type(_type),
                                                              lval(_lval),
                                                              rval(_rval) {}
    virtual void print_self(output &out) const {
        out << basic_expr(type) << " " << basic_expr(lval) << " = "
            << basic_expr(rval) << ";\n";
    }
    CLONE(decl_assignment_clause)
};

}
