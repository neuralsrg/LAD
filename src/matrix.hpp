#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cmath>
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>

class Matrix
{
    unsigned m, n;
    size_t shortest_cover;

    boost::dynamic_bitset<uint8_t> bitset, mask;
    std::vector<boost::dynamic_bitset<uint8_t>> covers;

    void gen_mask_cols(const std::vector<size_t>&);
    void gen_mask_rows(const std::vector<size_t>&);
    bool check_cover(const boost::dynamic_bitset<uint8_t>&);

    bool check_cols(const std::vector<size_t>&);
    bool check_cols(const boost::dynamic_bitset<uint8_t>&);

    public:

    Matrix(unsigned, unsigned, const std::vector<u_int8_t>&);
    void mine_covers();
    void print_covers();

    friend std::ostream& operator<<(std::ostream&, const Matrix&);
};

std::ostream& operator<<(std::ostream&, const Matrix&);

#endif