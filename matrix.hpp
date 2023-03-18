#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>

class Matrix
{
    unsigned m, n;
    boost::dynamic_bitset <uint8_t> bitset;

    public:

    Matrix(unsigned, unsigned, std::shared_ptr<std::vector<u_int8_t>>);
    friend std::ostream &operator<<(std::ostream &cout, const Matrix &);
};

std::ostream &operator<<(std::ostream &, const Matrix &);

#endif