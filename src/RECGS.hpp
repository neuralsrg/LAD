#ifndef GREEDY_MODIFICATION
#define GREEDY_MODIFICATION

#include <map>
#include "ECGS.hpp"

class RECGS : public ECGS
{
private:
    void print(string message, vector<size_t> nums) const;
    void reduceCoverage(dynamic_bitset<unsigned char>& coverage, const size_t& include, map<size_t, vector<size_t>>& supportRows, map<int, size_t>& intervals) const;
protected:
    void updateSupportRows(const vector<size_t>& exclude, size_t include, map<size_t, vector<size_t>>& supportRows) const;
    map<int, size_t> getNumIntervals(const dynamic_bitset<unsigned char>& coverage) const;
    bool gain(const vector<size_t>& exclude, size_t include, dynamic_bitset<unsigned char>& coverage, map<int, size_t>& intervals, map<size_t, vector<size_t>>& supportRows) const;
    vector<vector<size_t>> getLargestGroups(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals) const;
    void removeHeaviestCol(vector<size_t>& group, map<size_t, vector<size_t>> supportRows) const;
    // vector<vector<size_t>> getLightestCols(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals, map<size_t, vector<size_t>> supportRows) const;
    map<size_t, vector<size_t>> getSupportRows(const dynamic_bitset<unsigned char>& coverage) const;
    vector<size_t> getCoveringCols(const vector<size_t>& cols, const map<size_t, vector<size_t>>& supportRows) const;

public:
    RECGS(string filename);
    virtual ~RECGS() {};
    virtual dynamic_bitset<unsigned char> solve() override;
};

#endif