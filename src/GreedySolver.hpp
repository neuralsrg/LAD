#ifndef GREEDYSOLVER_HPP
#define GREEDYSOLVER_HPP

#include <set>
#include <boost/iterator/counting_iterator.hpp>
#include "BaseSolver.hpp"

class GreedySolver : public BaseSolver
{
private:
    void addColumn(vector<int>& covers_uncovered, dynamic_bitset<unsigned char>& coverage, set<int>& uncovered_rows, int newFeature);
public:
    GreedySolver(string filename);
    virtual ~GreedySolver() {};
    virtual dynamic_bitset<unsigned char> solve() override;
};


class HeapComparator
{
public:
    bool operator()(const pair<int, int&>& a, const pair<int, int&>& b) const { return a.second > b.second; }
};

#endif