#ifndef ECGS_HPP
#define ECGS_HPP

#include <boost/iterator/counting_iterator.hpp>
#include "BaseSolver.hpp"

class ECGS : public BaseSolver
{
private:
public:
    ECGS(string filename);
    virtual ~ECGS() {};
    virtual dynamic_bitset<unsigned char> solve() override;
};

#endif