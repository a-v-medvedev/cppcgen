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

#include "cppcgen_expr.h"
#include "cppcgen_exprdir.h"
#include "cppcgen_helpers.h"

namespace cppcgen {

static const size_t BR_NONE = 0;
static const size_t BR_OPEN = 1;
static const size_t BR_OPENSUB = 2;
static const size_t BR_CLOSE = 3;
static const size_t BR_OPENSUBST = 4;

struct token {
    size_t start;
    size_t len;
    size_t stop;
    size_t type;
};

void subst::act(std::string &str) {
    if (str.empty())
        return;

    if (from.empty()) {
        for (size_t i = 0; i < subordinate.size(); i++) {
            subordinate[i].act(str);
        }
        return;
    }

    size_t max_level = 0;
    {
        size_t L = 1;
        for (size_t i = 0; i < str.length(); i++) {
            if (str[i] == '{') {
                L++;
                if (max_level < L)
                    max_level = L;
            } else if (str[i] == '}') {
                if (L <= 1) { std::runtime_error("subst::act: internal error"); }
                --L;
            }
        }
        if (L != 1) { std::runtime_error("subst::act: internal error"); }
    }

    if (max_level == 0) {
        for (size_t i = 0; i < subordinate.size(); i++) {
            subordinate[i].act(str);
        }
        return;
    }

    std::vector<token> tokens;
    for (size_t cnt = max_level - 1; cnt > 0; cnt--) {
        std::vector<pair<size_t> > brackets;
        for (size_t i = 0; i < str.length(); i++) {
            brackets.push_back(pair<size_t>());
        }
        size_t level = 1;
        size_t new_max_level = 0;
        for (size_t i = 0; i < str.length(); i++) {
            if (str[i] == '{') {
                size_t Type = BR_OPEN;
                if (i && str[i - 1] == '.')
                    Type = BR_OPENSUB;
                if (i && str[i - 1] == '$') 
                    Type = BR_OPENSUBST;
                brackets[i] = pair<size_t>(Type, level++);
                if (new_max_level < level)
                    new_max_level = level;
            } else if (str[i] == '}') {
                if (level <= 1) { std::runtime_error("subst::act: internal error"); }
                brackets[i] = pair<size_t>(BR_CLOSE, --level);
            } else {
                brackets[i] = pair<size_t>(BR_NONE, 0);
            }
        }

        cnt = new_max_level > 0 ? new_max_level - 1 : 0;

        if (level != 1) { std::runtime_error("subst::act: internal error"); }
        tokens.clear();
        token tk;
        bool opened = false;
        size_t currlen = 0;
        for (size_t i = 0; i < str.length(); i++) {
            if (brackets[i].first == BR_NONE) {
                if (opened && tk.len == 0) {
                    currlen++;
                    if (str[i] == '.' || str[i] == '#' || 
                            str[i] == '\1' || str[i] == '\2') {
                        tk.len = currlen;
                    }
                }
                continue;
            }
            if (brackets[i].second == cnt) {
                if (brackets[i].first == BR_OPEN || 
                        brackets[i].first == BR_OPENSUB ||
                        brackets[i].first == BR_OPENSUBST) {
                    if (opened != false) { std::runtime_error("subst::act: internal error"); }
                    opened = true;
                    tk.start = i;
                    tk.len = 0;
                    tk.stop = 0;
                    tk.type = brackets[i].first;
                    currlen = 0;
                } else if (brackets[i].first == BR_CLOSE) {
                    if (opened != true) { std::runtime_error("subst::act: internal error"); }
                    opened = false;
                    tk.stop = i;
                    if (tk.len == 0)
                        tk.len = currlen;
                    tokens.push_back(tk);
                }
            } else {
                tk.len = currlen;
            }
        }

        if (tokens.size() == 0)
            continue;
        for (size_t j = tokens.size(); j; j--) {
            token &t = tokens[j - 1];
            if (!t.len)
                continue;
            if (t.type == BR_OPENSUBST) {
                str[t.start] = '\1';
                str[t.stop] = '\2';
                continue;
            }
            std::string value = Mid(str, t.start + 1, t.len);
            if (value[value.length() - 1] == '.')
                Delete(value, value.length() - 1, 1);
//	  printf("Value = <%s>;%d, From = <%s>;%d\n", Value.c_str(), Value.length(), from.c_str(), from.length());
            if (value == from) {
                str[t.start] = '#';
                str[t.stop] = '#';
                Delete(str, t.start + 1, t.len);
                Insert(str, to, t.start + 1);
            } else {
                str[t.start] = '\1';
                str[t.stop] = '\2';
            }
        }
    }

    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '\1')
            str[i] = '{';
        if (str[i] == '\2')
            str[i] = '}';
    }
    for (size_t i = 0; i < subordinate.size(); i++) {
        subordinate[i].act(str);
    }
}


void subst::remove_syntax_helpers(std::string &final_string) {
    Replace(final_string, "#", "");
}

std::string expr::render(std::vector<subst> &Substs) {
//    printf("RENDER: <%s>\n", str.c_str());
    if (str.empty())
        return str;
    std::string result = str;
    bool isdone = true;
    unsigned maxiter = 20;
    std::string prev_result = result;
    do {
        for (size_t i = 0; i < Substs.size(); i++) {
            Substs[i].act(result); 
        }
        isdone = true;
        for (size_t i = 0; i < result.length(); i++) {
            if (result[i] == '{' || result[i] == '}') {
                isdone = false;
            }
        }
        if (!isdone) {
            fix_syntax(result);
            if (result == prev_result)
                break;
            prev_result = result;
        }
    } while (!isdone && --maxiter);
    if (!maxiter) {
        printf("WARNING: limit on iterations reached in Render: <%s>\n", result.c_str());
    }
//    printf("RENDER: result: <%s>\n", result.c_str());
    return result;
}

void expr::fix_syntax(std::string &final_string) {
    subst::remove_syntax_helpers(final_string);
}

}
