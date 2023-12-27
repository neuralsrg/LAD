#include "parse_csv.hpp"

params::Parser::Parser(std::string filename, std::string tfilename)
{
    rfile.open(filename);
    tfile.open(tfilename);
    assert(rfile && tfile);
}

params::Parser::~Parser()
{
    rfile.close();
    tfile.close();
}

params::TMatrix params::Parser::parseCsv(bool full)
{
    std::string                         row;
    params::TMatrix                     res;
    std::vector<std::bitset<params::n>> row_mat(full ? params::full_m : params::m);
    std::vector<std::bitset<params::m>> col_mat(full ? params::full_n : params::n);
    int                                 i = -1;
    while(std::getline(rfile, row))
    {
        ++i;
        row.erase(std::remove(row.begin(), row.end(), ','), row.end());
        row_mat[i] = std::bitset<params::n>(row);
    }
    i = -1;
    while(std::getline(tfile, row))
    {
        ++i;
        row.erase(std::remove(row.begin(), row.end(), ','), row.end());
        col_mat[i] = std::bitset<params::m>(row);
    }
    return params::TMatrix {row_mat, col_mat};
}