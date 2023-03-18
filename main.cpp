#include <iostream>
#include "vector_generator.hpp"
#include "matrix.hpp"

int main()
{
    // long N = 10000;
    // int num = 0b110;
    // boost::dynamic_bitset <uint8_t> B (N, num);
    // std::cout << B << std::endl;
    VectorGenerator generator(0.7);
    auto v = generator(100);

    Matrix mtr(10, 10, v);
    std::cout << mtr << std::endl;

    return 0;
}