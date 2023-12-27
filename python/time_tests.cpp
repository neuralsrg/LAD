#include <iostream>
#include <chrono>

#include "vector_generator.hpp"
#include "matrix.hpp"

using namespace std::chrono;

int main()
{
    try {
        freopen("logs.csv", "w", stdout);

        std::vector<unsigned> columns {3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25};
        std::vector<float> probs {0.1, 0.3, 0.5, 0.7, 0.9};
        std::vector<unsigned> rows {29, 22, 23, 24};

        std::cout << "Rows,Time (ms)" << std::endl;

        /*
        for (int i = 0; i < 5; ++i) {
            for (auto& p: probs) {
                for (auto& n: columns) {

                    unsigned m = 50;

                    VectorGenerator generator(p);
                    auto v = generator(m*n);
                    Matrix mtr(m, n, v);

                    auto start = high_resolution_clock::now();
                    mtr.mine_covers();
                    auto stop = high_resolution_clock::now();
                    auto duration = duration_cast<milliseconds>(stop - start);

                    std::cout << p << "," << int(n) << "," << duration.count() << std::endl;
                }
            }
        }
        */
        for (int i = 0; i < 5; ++i) {
            for (auto& tmp: rows) {
                unsigned m = tmp;
                unsigned n = tmp;

                VectorGenerator generator(0.3);
                auto v = generator(m*n);
                Matrix mtr(m, n, v);

                auto start = high_resolution_clock::now();
                mtr.mine_covers();
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);

                std::cout << int(m) << "," << duration.count() << std::endl;
            }
        }

        fclose(stdout);
        return 0;
    }
    catch (char const* exc) {
        std::cout << "Exception: " << exc << std::endl;
    }
}