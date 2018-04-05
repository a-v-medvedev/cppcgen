/*
    Copyright (C) 2011-2013,2018 Alexey V. Medvedev

    This file is part of cppcgen. cppcgen is a C++-11 generator of C code with macroses.

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


#include "cppcgen_expr.h"
#include "cppcgen_exprdir.h"
#include "cppcgen_helpers.h"

namespace cppcgen {

static int FromString(std::string &str)
{
    return atoi(str.c_str());
}

static std::string ToString(int i)
{
    std::string r;
    Format(r, "%d", i);
    return r;    
}

 
struct eval_style_token {
    size_t start;
    size_t bracket1;
    size_t bracket2;
    size_t separator;
    size_t separator2;
    std::string name;
    size_t get_args(std::string token, std::string &Arg1, std::string &Arg2, std::string &Arg3) {
        if (separator == 0) {
            Arg1 = Mid(token, bracket1 + 1, bracket2 - 1 - bracket1);
            return 1;
        }
        Arg1 = Mid(token, bracket1 + 1, separator - bracket1 - 1);
        if (separator2) {
            Arg2 = Mid(token, separator + 1, separator2 - 1 - separator);
            Arg3 = Mid(token, separator2 + 1, bracket2 - 1 - separator2);
            return 3;
        } else {
            Arg2 = Mid(token, separator + 1, bracket2 - 1 - separator);
            return 2;
        }
    }
};


bool expression::parse_function(std::string name, std::string &str, char *t, 
                                eval_style_token &token) 
{
    const char OPEN_BRACKET = '{';
    const char CLOSE_BRACKET = '}';
    const char ARG_DELIMITER = '@';
    char *p = t + 1 + name.length();
    if (*p != OPEN_BRACKET)
        return false;
    token.name = name;
    token.start = t - str.c_str();
    token.bracket1 = p - str.c_str();
    token.separator = 0;
    token.separator2 = 0;
    p++;
    const unsigned INITIAL = 0;
    const unsigned FIRST_ARG_FOUND = 1;
    const unsigned SECOND_ARG_FOUND = 2;
    const unsigned STOPPED = 3;
    unsigned mode = INITIAL;
    unsigned level = 1;
    do {
        if (*p == OPEN_BRACKET) {
            level++;
        }	
        if (*p == ARG_DELIMITER && level == 1) {
            if (mode == INITIAL) {
                token.separator = p - str.c_str();
                mode = FIRST_ARG_FOUND;
                continue;
            } else if (mode == FIRST_ARG_FOUND) {
                token.separator2 = p - str.c_str();
                mode = SECOND_ARG_FOUND;
            } else {
                return false;
            }
        }
        if ((*p == CLOSE_BRACKET) && (level-- == 1)) {
            // if (mode != FIRST_ARG_FOUND && mode != SECOND_ARG_FOUND)
            //     return false;
            mode = STOPPED;
            token.bracket2 = p - str.c_str();
            break;
        }
    } while (*p++);
    if (mode == STOPPED)
        return true;
    return false;
}

std::string expression::translate(std::string classes) 
{
    std::vector<subst> substs;
    ssize_t n = 0;
    size_t maxiter = 5;
    std::string result;

    do {
        const char *t = strstr(classes.c_str() + n, "Class:");
        if (t != NULL) {
            std::string c = find_option(std::string(t), "Class:");
            if (c.empty()) { std::runtime_error("Translate: incorrect Class option"); }
            std::vector<subst> item = expression_directory::get_subst_set(c);
            substs.push_back(subst(item));
        } else {
            t = strstr(classes.c_str() + n, "Subst:");
            if (t != NULL) {
                std::string subst_str = find_option(std::string(t), "Subst:");    
                if (subst_str.empty()) { std::runtime_error("Translate: incorrect Subst option"); }
                const char *p = strchr(subst_str.c_str(), '=');
                if (p != NULL) {
                    std::string to(p + 1);
                    std::string from = Mid(subst_str, 0, p - subst_str.c_str());
                    subst s(from, to);
                    substs.push_back(s);
                }
            } else {
                t = classes.c_str() + n + 1;
            }
        }

        const char *next_ptr = strchr(t, ' ');
        if (next_ptr == NULL)
            break;
        n = next_ptr - classes.c_str();
    } while(true);

    std::string to_process = str;
    do {
        expr E(to_process);
        result = E.render(substs);
        E.fix_syntax(result);

        std::string result2 = expand(result, classes);
        if (result == result2) 
            break;
        to_process = result2;
    } while (--maxiter);

    if (!maxiter) {
        printf("WARNING: limit on iterations reached in Translate: <%s>\n", to_process.c_str());
    }
    return result;
}

std::string expression::expand(const std::string str, std::string &classes) 
{
//        printf("EXPAND: <%s> classes: <%s>\n", str.c_str(),  classes.c_str());
    std::string result = str;
    const char *s = result.c_str();
    std::vector<eval_style_token> tokens;
    while (true) {
        char *t = strchr((char *)s, '$');
        if (t == NULL)
            break;
        std::vector<std::string> names;
        names.push_back("EVAL$");
        names.push_back("$"); // a synonim for EVAL
        names.push_back("EACH$");
        names.push_back("SEQ$");
        size_t n = 0;
        for (; n < names.size(); n++) {
            int r = strncmp(t + 1, names[n].c_str(), names[n].length());
            if (r)
                continue;
            eval_style_token token;  
            bool res = parse_function(names[n], result, t, token);
            if (res) {
                tokens.push_back(token);
                s = result.c_str() + token.bracket2 + 1;
                break;
            } else {
                std::string msg;
                Format(msg, "$%s operator syntax error", names[n].c_str());
                throw std::runtime_error(msg.c_str());
            }
        }
        if (n == names.size()) {
            s++;
        }
    }

    // in back-direction, so we keep addresses in tokens ok 
    for (size_t i = tokens.size(); i > 0; i--) {
        eval_style_token &t = tokens[i - 1];
        std::string arg1, arg2, arg3;
        size_t nargs;
        nargs = t.get_args(result, arg1, arg2, arg3);

        Trim(arg1);
        unsigned maxiter = 5;
        while (!(strchr(arg1.c_str(), '$') == NULL) && --maxiter) {
            std::string new_arg1 = expand(arg1, classes);
            arg1 = new_arg1;
            //continue;
        }
        if (!maxiter) {
            printf("WARNING: limit on iterations reached in expand: <%s>\n", arg1.c_str());
        }

        Trim(arg2);
        maxiter = 5;
        while (!(strchr(arg2.c_str(), '$') == NULL) && --maxiter) {
            std::string new_arg2 = expand(arg2, classes);
            arg2 = new_arg2;
            //continue;
        }
        if (!maxiter) {
            printf("WARNING: limit on iterations reached in expand: <%s>\n", arg2.c_str());
        }

        if (t.name == std::string("EVAL$") || t.name == std::string("$")) {
            if (nargs != 2) { std::runtime_error("expand: $EVAL$ syntax error"); }

            expression sub_expr(arg1);
            Helpers::AddWordsToStr("Subst:", arg2);
            Trim(arg2);
//       	    printf("TRANSLATE: <%s> WITH <%s>\n", arg1.c_str(), arg2.c_str());
            std::string new_classes = classes + std::string(" ") + arg2;
            std::string subresult = sub_expr.translate(new_classes);
            Delete(result, t.start, t.bracket2 - t.start + 1);
            Insert(result, subresult, t.start);  
//       	    printf("TRANSLATE: result: <%s>\n", result.c_str());
        } else if (t.name == std::string("EACH$")) {
//                    printf(">> EACH BEGIN: %s %s %s\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
            bool skip = false;
            if (nargs <= 1) { std::runtime_error("expand: $EACH$ syntax error"); }
            std::string subresult;
            std::vector<std::string> iter;
            //arg2.Trim();
            std::vector<std::string> vars;
            std::vector<std::string> strs;
            size_t m = Helpers::Split(arg2, ';', vars);
            size_t prev_n = 0;
            for (size_t k = 0; k < m; k++) {
                size_t n = Helpers::Split(vars[k], ',', iter);
                if (prev_n && n != prev_n) { 
                    throw std::runtime_error("$EACH$ operator: different number of substitutors");
                }
                if (prev_n == 0) { for (size_t z = 0; z < n; z++) strs.push_back(""); }
                prev_n = n;

                // arg2 is unexpanded macro -- skip this replacement
                if (strchr(arg2.c_str(), '{') != NULL) {
                    skip = true;
                    //printf("SKIPPED: %s %s %s\n", (char *)arg1, (char *)arg2, (char *)arg3);
                    break;
                }
                if (strchr(arg2.c_str(), '$') != NULL) {
                    throw std::runtime_error("$EACH$ operator: no $-operator allowed inside an operator");
                }
                for (size_t z = 0; z < n; z++) {
                    //std::string str;
                    Trim(arg1);
                    Trim(iter[z]);
                    if (strchr(iter[z].c_str(), '=') == NULL) {
                        unsigned j = z;
                        while (j--) {
                            const char *p;

                            p = strchr(iter[j].c_str(), '='); 
                            if (p != NULL) {
                                std::string left = Mid(iter[j], 0, p - iter[j].c_str() + 1);
                                iter[z] = left + iter[z];
                                break;
                            }
                        } 
                    }
                    if (strs[z] == "") strs[z] = arg1;
                    std::string str;
                    Format(str, "$${%s @ %s}", strs[z].c_str(), iter[z].c_str());
                    strs[z] = str;
                }
            }
            if (skip)
                continue;
            for (size_t z = 0; z < prev_n; z++) {
                if (z != prev_n - 1)
                    strs[z] += arg3;
                subresult += strs[z];
            }
            Delete(result, t.start, t.bracket2 - t.start + 1);
            Insert(result, subresult, t.start); 
        } else if (t.name == std::string("SEQ$")) {
            if (nargs != 1) {
                throw std::runtime_error("$SEQ$ operator: syntax error");
            }
            Trim(arg1);
            // arg2 is unexpanded macro -- skip this replacement
            if (strchr(arg1.c_str(), '{') != NULL)
                continue;
            if (strchr(arg2.c_str(), '$') != NULL) {
                throw std::runtime_error("$SEQ$ operator: no $-operator allowed inside an operator");
            }

            // std::vector<std::string> LeftAndRightSides;
            // size_t n = Helpers::Split(arg1, '=', LeftAndRightSides);
            // if (n != 2)
            //     throw std::runtime_error("$SEQ$ operator: syntax error");
            // std::string &Var = LeftAndRightSides[0];
            std::string &seq = arg1;

            // Seq -> A[0],A[1]
            // A[0] -> B[0]..B[2]
            std::vector<std::string> A;
            size_t m = Helpers::Split(seq, ',', A);
            if (m == 1)
                A.push_back("1");
            else if (m != 2)
                throw std::runtime_error("$SEQ$ operator: syntax error");
            std::vector<std::string> B;
            size_t k = Helpers::Split(seq, '.', B);
            if (k != 3)
                throw std::runtime_error("$SEQ$ operator: syntax error");
            std::string subresult;
            for (int z = FromString(B[0]); z <= FromString(B[2]); z += FromString(A[1])) {
                if (z != FromString(B[0]))
                    subresult += ",";
                subresult += ToString(z);
            }
            //          Subresult = Var + "=" + Subresult;

            Delete(result, t.start, t.bracket2 - t.start + 1);
            Insert(result, subresult, t.start);
        }
    }
    return result;
}

std::string expression::find_option(std::string MyOptions, std::string Opt) {
    const char *p = strstr(MyOptions.c_str(), Opt.c_str());
    if (p == NULL)
        return std::string("");
    size_t n = p - MyOptions.c_str();
    std::string result = Mid(MyOptions, n + Opt.length());
    size_t l = Find(result, ' ');  
    result = Left(result, l);
    return result;  
}

}
