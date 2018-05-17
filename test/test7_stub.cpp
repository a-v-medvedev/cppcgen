#include <iostream>
#include <vector>
#include <string>
#include <stdarg.h>
#include <assert.h>

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

void print(const std::string &header, const std::vector<double> &a, size_t d1, size_t d2)    
{
    std::cout << header << std::endl;
    for (size_t i = 0; i < d1; i++) {
        for (size_t j = 0; j < d2; j++) {
            std::cout << Format("%02d", (int)a[i*d2 + j]) << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void update_boundary_1C_2D(double *, size_t, size_t);
void update_boundary_2C_2D(double *, size_t, size_t);

void fill_for_test(std::vector<double> &array, int ncells, size_t d1, size_t d2) {
    assert(ncells == 2 || ncells == 1);
    assert(d1 > 4 && d2 > 4);
    double filler = 10.0;
    array.assign(d1 * d2, 0.0);
    for (size_t i = 0; i < d1; i++) {
        for (size_t j = 0; j < d2; j++) {
            if (i != 0 && j != 0 && i != d1-1 && j != d2-1)
                if (ncells != 2 || (i != 1 && j != 1 && i != d1-2 && j != d2-2))
                    array[i*d2 + j] = (filler += 1.0);
        }
    }
}

int main(int argc, char **argv)
{
    const size_t d1 = 8, d2 = 8;
    std::vector<double> array;
    fill_for_test(array, 1, d1, d2);
    print("Before boundary update (1-cell boundary):", array, d1, d2);
    update_boundary_1C_2D(&(array[0]), d1, d2);
    print("After boundary update:", array, d1, d2);

    fill_for_test(array, 2, d1, d2);
    print("Before boundary update (2-cells boundary):", array, d1, d2);
    update_boundary_2C_2D(&(array[0]), d1, d2);
    print("After boundary update:", array, d1, d2);

    return 0;
   
}

