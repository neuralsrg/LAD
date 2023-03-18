#ifndef VECTOR_GENERATOR_HPP
#define VECTOR_GENERATOR_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <algorithm>

class VectorGenerator
{
    float prob;

    std::random_device rd;
    std::mt19937 generator;
    std::bernoulli_distribution dist;

    public:

    VectorGenerator(float prob);
    std::shared_ptr<std::vector<uint8_t>> operator()(size_t length);
    static void print_vector(std::shared_ptr<std::vector<u_int8_t>>);
};

#endif