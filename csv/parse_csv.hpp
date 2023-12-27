#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <bitset>
#include <cassert>

namespace params
{
    /* Set them according to dataset size */
    const int full_m = 25, full_n = 30;
    const int m = 5, n = full_n;

    using TBitRow = std::bitset<n>;
    using TBitCol = std::bitset<m>;
    struct TMatrix
    {
        std::vector<TBitRow> row_mat = std::vector<TBitRow>(m);
        std::vector<TBitCol> col_mat = std::vector<TBitCol>(n);
    };
    class Parser;
}

class params::Parser
{
private:
    std::ifstream       rfile;
    std::ifstream       tfile;
public:
    Parser(std::string filename, std::string tfilename);
    ~Parser();
    params::TMatrix parseCsv(bool full=false);
};

#endif