#ifndef GREEDY_MODIFICATION
#define GREEDY_MODIFICATION

#include <map>
#include "GreedySolver.hpp"

class GreedyModification : public GreedySolver
{
protected:
    map<int, size_t> getNumIntervals(const dynamic_bitset<unsigned char>& coverage) const;
    bool gain(const vector<int>& exclude, int include, const dynamic_bitset<unsigned char>& coverage, map<int, size_t>& intervals) const;
    vector<size_t> getLargestGroup(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals) const;
    void removeHeaviestCol(vector<size_t>& group, map<size_t, vector<size_t>> supportRows) const;
    vector<size_t> getLightestCols(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals, map<size_t, vector<size_t>> supportRows) const;
    map<size_t, vector<size_t>> getSupportRows(const dynamic_bitset<unsigned char>& coverage) const;
    vector<size_t> getCoveringCols(const vector<size_t>& cols, const map<size_t, vector<size_t>>& supportRows) const;

public:
    GreedyModification(string filename);
    virtual ~GreedyModification() {};
    virtual dynamic_bitset<unsigned char> solve() override;
};

#endif