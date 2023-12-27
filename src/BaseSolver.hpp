#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace boost;


struct TMatrix
{
    vector<dynamic_bitset<uint8_t>> row_mat;
    vector<dynamic_bitset<uint8_t>> col_mat;
};

class BaseSolver
{
private:
    void parseCsv(string filename);
    void buildColMat();
    void readHeader(string headerfile);
protected:
    TMatrix mat;
    vector<int> header;
public:
    BaseSolver(string filename, string headerfile);
    void printMatrix();
};