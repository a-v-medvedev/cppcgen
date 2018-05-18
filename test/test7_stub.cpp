#include <iostream>
#include <vector>
#include <string>
#include <stdarg.h>
#include <assert.h>
#include <math.h>

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
    for (size_t i = 0; i < d2; i++) {
        for (size_t j = 0; j < d1; j++) {
            std::cout << Format("%02d", (int)a[i*d1 + j]) << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void update_boundary_1C_0O_2D(double *, size_t, size_t);
void update_boundary_2C_0O_2D(double *, size_t, size_t);
void update_boundary_1C_1O_2D(double *, size_t, size_t);
void update_boundary_2C_1O_2D(double *, size_t, size_t);
void update_boundary_1C_2O_2D(double *, size_t, size_t);
void update_boundary_2C_2O_2D(double *, size_t, size_t);

bool is_bound(int ncells, size_t j, size_t i, size_t d2, size_t d1) {
    assert(ncells >= 0);
    bool res = false;
    do 
        res = res || (j == ncells-1 || i == ncells-1 || j == d2-ncells || i == d1-ncells);
    while (--ncells);
    return res;
}

void fill_for_test(std::vector<double> &array, int ncells, size_t d1, size_t d2) {
    assert(ncells >= 0 && ncells <= d1/2 && ncells <= d2/2);
    assert(d1 > 4 && d2 > 4);
    double filler = 10.0;
    array.assign(d1 * d2, 0.0);
    for (size_t j = 0; j < d2; j++) {
        for (size_t i = 0; i < d1; i++) {
            if (!is_bound(ncells, j, i, d2, d1))
                array[j*d1 + i] = (filler + sqrt((j-d2/2)*(j-d2/2) + (i-d1/2)*(i-d1/2)));
        }
    }
}

int main(int argc, char **argv)
{
    const size_t d1 = 11, d2 = 21;
    std::vector<double> array;
    fill_for_test(array, 1, d1, d2);
    print("Before boundary update (1-cell boundary):", array, d1, d2);
    update_boundary_1C_0O_2D(&(array[0]), d1, d2);
    print("After boundary update (zero-order, copy):", array, d1, d2);
    fill_for_test(array, 1, d1, d2);
    update_boundary_1C_1O_2D(&(array[0]), d1, d2);
    print("After boundary update (1st-order, linear approx.):", array, d1, d2);
    fill_for_test(array, 1, d1, d2);
    update_boundary_1C_2O_2D(&(array[0]), d1, d2);
    print("After boundary update (2nd-order, Newton approx.):", array, d1, d2);

    fill_for_test(array, 2, d1, d2);
    print("Before boundary update (2-cells boundary):", array, d1, d2);
    update_boundary_2C_0O_2D(&(array[0]), d1, d2);
    print("After boundary update (zero-order, copy):", array, d1, d2);
    fill_for_test(array, 2, d1, d2);
    update_boundary_2C_1O_2D(&(array[0]), d1, d2);
    print("After boundary update (1st-order, linear approx.):", array, d1, d2);
    fill_for_test(array, 2, d1, d2);
    update_boundary_2C_2O_2D(&(array[0]), d1, d2);
    print("After boundary update (2nd-order, Newton approx.):", array, d1, d2);

    return 0;
}

