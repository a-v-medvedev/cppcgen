#include <iostream>
#include <vector>
#include <string>
#include <stdarg.h>

static inline void FormatV(std::string &str, const char *Format, va_list SourceArgList) {
    std::vector<char> buffer;
    buffer.resize(1024);
    ssize_t count;
    while ((count = vsnprintf(&buffer[0], 1024, Format, SourceArgList)) < 0 ||
            static_cast<size_t>(count) >= 1024)
        buffer.resize(buffer.size() + 1024);
    if (count) {
        buffer.resize(count);
        str.assign(&buffer[0], buffer.size());
    }
    else str.assign("");
}

static inline std::string  Format(const char *Format, ...) {
    std::string str;
    va_list argList;
    va_start(argList, Format);
    FormatV(str, Format, argList);
    va_end(argList);
    return str;
}

void print(const std::vector<double> &a, size_t d1, size_t d2)    
{
    std::cout << std::endl;
    for (size_t i = 0; i < d1; i++) {
        for (size_t j = 0; j < d2; j++) {
            std::cout << Format("%02d", (int)a[i*d2 + j]) << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void update_boundary_2D(double *, size_t, size_t);

int main(int argc, char **argv)
{
    double filler = 10.0;
    const size_t d1 = 8, d2 = 8;
    std::vector<double> array;
    array.assign(d1 * d2, 0.0);
    for (size_t i = 0; i < d1; i++) {
        for (size_t j = 0; j < d2; j++) {
            if (i != 0 && j != 0 && i != d1-1 && j != d2-1)
//                Uncomment for bthick=2 case:
//                if (i != 1 && j != 1 && i != d1-2 && j != d2-2)
                    array[i*d2 + j] = (filler += 1.0);
        }
    }
    print(array, d1, d2);
    update_boundary_2D(&(array[0]), d1, d2);
    print(array, d1, d2);
    return 0;
}
