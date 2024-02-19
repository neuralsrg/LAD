#ifndef GREEDY_HPP
#define GREEDY_HPP

#include <boost/iterator/counting_iterator.hpp>
#include "BaseSolver.hpp"

class Greedy : public BaseSolver
{
public:
    Greedy(string filename);
    virtual ~Greedy() {};
    virtual dynamic_bitset<unsigned char> solve() override;
};

#endif