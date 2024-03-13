#ifndef HEURISTIC_OPT
#define HEURISTIC_OPT

#include "RECGS.hpp"
#include <numeric>

class HeuristicOpt : public RECGS
{
private:
    bool checkGain(size_t exclude, size_t include, const map<int, size_t>& intervals) const;
    void updateStatistics(size_t exclude, size_t include,
        map<size_t, vector<size_t>>& supportRows, map<int, size_t>& intervals) const;
    bool checkHeuristicGain(const vector<size_t>& coverageCols, size_t include) const;
    double computeHeuristic(const vector<size_t>& coverageCols) const;

public:
    HeuristicOpt(string filename);
    virtual ~HeuristicOpt() {};
    virtual dynamic_bitset<unsigned char> solve() override;
};

#endif