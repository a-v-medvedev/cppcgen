#pragma once

namespace cppcgen {

typedef expression_directory dir;
typedef std::pair<const std::string, const std::string> macro;

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

static inline int translate_to_int(std::string expr)
{
    return from_string(basic_expr(expr).translate());
}

}
