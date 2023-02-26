#include "vector_generator.hpp"

VectorGenerator::VectorGenerator(float prob) : 
    prob(prob), rd(), generator(rd()), dist(prob) {}

std::shared_ptr<std::vector<char>> VectorGenerator::operator()(size_t length)
{
    auto v = std::make_shared<std::vector<char>>(length);
    auto set_value = [&](auto &val) { val = VectorGenerator::dist(VectorGenerator::generator); };
    std::for_each((*v).begin(), (*v).end(), set_value);
    return(v);
}

void VectorGenerator::print_vector(std::shared_ptr<std::vector<char>> v)
{
    for(const auto &val : *v) {
        std::cout << int(val) << ' ';
    }
    std::cout << std::endl;
}