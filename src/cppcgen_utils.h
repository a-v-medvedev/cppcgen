#pragma once

namespace cppcgen {

typedef expression_directory dir;
typedef std::pair<const std::string, const std::string> macro;
typedef if_clause if_;
typedef else_clause else_;
typedef return_clause return_;
typedef for_clause for_;
typedef function_clause function_;
typedef assignment_clause assign_;
typedef switch_clause switch_;
typedef case_clause case_;
typedef break_clause break_;
typedef continue_clause continue_;
typedef default_clause default_;
typedef call_clause call_;
typedef define_clause define_;

static inline std::vector<macro> format_macroses(const char *f, const char *t,
                                                 const std::vector<macro> &v)
{
    std::vector<macro> m;
    for (auto it = v.begin(); it != v.end(); it++) {
        m.push_back(macro { Format(f, it->first.c_str()), Format(t, it->second.c_str()) });
    }
    return m;
}

#define FOR_EACH_IN_LIST(ITER, SIZE, ARR, EXPR) std::vector<std::string> ARR; \
                                    Helpers::Split(basic_expr(EXPR).translate(), ',', ARR); \
                                    size_t SIZE = ARR.size(); \
                                    for (size_t ITER = 0; ITER < SIZE; ITER ++)

static inline int macro_to_int(const std::string &expr)
{
    return from_string(basic_expr(expr).translate());
}

static inline macro int_to_macro(const std::string &macro_name, int i) {
    return macro { macro_name, Format("%d", i) };
}

std::vector<macro> &operator +=(std::vector<macro> &to,
                                const std::vector<macro> &from)
{
    for (auto &m : from)
        to.push_back(m);
    return to;
}

std::vector<macro> &operator +=(std::vector<macro> &to,
                                const macro &from)
{
    to.push_back(from);
    return to;
}

}
