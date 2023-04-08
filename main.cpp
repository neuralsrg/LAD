#include <iostream>
#include "vector_generator.hpp"
#include "matrix.hpp"

int main()
{
    try {
        boost::dynamic_bitset<uint8_t> A (10, 0);

        VectorGenerator generator(0.3);
        auto v = generator(1500);
        /*
        v = {1, 0, 0, 0, 0,
             0, 1, 0, 0, 0,
             0, 0, 1, 0, 0,
             0, 0, 0, 0, 1,
             0, 0, 0, 1, 0};
        */
        Matrix mtr(50, 30, v);

        //boost::dynamic_bitset<uint8_t> cover (5, 5);
        
        std::cout << "Matrix:" << std::endl;
        std::cout << mtr << std::endl;

        //std::cout << std::boolalpha << mtr.check_cols(cover) << std::endl;
        mtr.mine_covers();
        mtr.print_covers();

        return 0;
    }
    catch (char const* exc) {
        std::cout << "Exception: " << exc << std::endl;
    }
}