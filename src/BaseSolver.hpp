#ifndef BASESOLVER_HPP
#define BASESOLVER_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace boost;


struct TMatrix
{
    vector<dynamic_bitset<unsigned char>> row_mat;
    vector<dynamic_bitset<unsigned char>> col_mat;
};

class BaseSolver
{
private:
    void parseCsv(string filename);
    void buildColMat();
protected:
    TMatrix mat;
    vector<int> header;
    bool checkCoverage(const dynamic_bitset<unsigned char>& coverage) const;
    void reduceCoverage(dynamic_bitset<unsigned char>& coverage);
    void addColumn(vector<int>& covers_uncovered, dynamic_bitset<unsigned char>& coverage, set<int>& uncovered_rows, int newFeature);
public:
    BaseSolver(string filename);
    virtual ~BaseSolver() {};
    void printMatrix() const;
    virtual dynamic_bitset<unsigned char> solve() = 0;
    string coverage2String(const dynamic_bitset<unsigned char>& coverage) const;
    int maxBins(const dynamic_bitset<unsigned char>& coverage) const;
};

#endif