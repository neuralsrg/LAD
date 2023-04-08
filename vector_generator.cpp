#include "vector_generator.hpp"

VectorGenerator::VectorGenerator(float prob) : 
    prob(prob), rd(), generator(rd()), dist(prob) {}

std::vector<u_int8_t> VectorGenerator::operator()(size_t length)
{
    std::vector<u_int8_t> v(length);
    auto set_value = [&](auto &val) { val = VectorGenerator::dist(VectorGenerator::generator); };
    std::for_each(v.begin(), v.end(), set_value);
    return v;
}

void VectorGenerator::print_vector(const std::vector<u_int8_t>& v)
{
    for(const auto& val: v) {
        std::cout << int(val) << ' ';
    }
    std::cout << std::endl;
}