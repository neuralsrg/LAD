#include "matrix.hpp"

Matrix::Matrix(unsigned m, unsigned n, std::shared_ptr<std::vector<u_int8_t>> v) :
    m(m), n(n), bitset(v->size(), 0)
{
    if (v->size() != m*n) {
        throw("Matrix shape is incompatible with given vector.");
    }

    std::size_t i = 0;
    std::for_each(v->begin(), v->end(), [&](uint8_t val)
        { bitset[i++] = val; }); 
}

std::ostream &operator<<(std::ostream &cout, const Matrix &matrix)
{
    for (unsigned i = 0; i < matrix.m; ++i) {
        for (unsigned j = 0; j < matrix.n; j++) {
            cout << matrix.bitset[i*matrix.n + j] << ' ';
        }
        cout << std::endl;
    }
    return cout;
}