#include "BaseSolver.hpp"

/* Constructor */

BaseSolver::BaseSolver(string filename, string headerfile) : mat(), header(0)
{
    parseCsv(filename);
    buildColMat();
    readHeader(headerfile);
}

/* Private members */

void BaseSolver::parseCsv(string filename)
{
    string row;
    ifstream boolfile;

    /* Openning file containing boolean matrix */
    boolfile.open(filename);
    while(getline(boolfile, row))
    {
        row.erase(remove(row.begin(), row.end(), ','), row.end());
        reverse(row.begin(), row.end()); // bitset[0] -> first column
        mat.row_mat.push_back(dynamic_bitset<unsigned char>(row));
    }
    boolfile.close();
}

void BaseSolver::buildColMat()
{
    for (size_t j = 0; j < mat.row_mat[0].size(); ++j) {
        mat.col_mat.push_back(dynamic_bitset<unsigned char>(mat.row_mat.size(), 0));
        for (size_t i = 0; i < mat.row_mat.size(); ++i) {
            mat.col_mat[j][i] = mat.row_mat[i][j];
        }
    }
}

void BaseSolver::readHeader(string headerfile)
{
    std::ifstream header_file;
    string token;

    header_file.open(headerfile);
    while (getline(header_file, token, ',')) {
        header.push_back(stoi(token));
    }
    header_file.close();
}

/* Public members */

void BaseSolver::printMatrix()
{
    string str;

    cout << "Row matrix" << endl;
    for (const dynamic_bitset<unsigned char>& row : mat.row_mat) {
        boost::to_string(row, str);
        reverse(str.begin(), str.end());
        cout << str << endl;
    }

    cout << "\nColumn matrix" << endl;
    for (const dynamic_bitset<unsigned char>& col : mat.col_mat) {
        boost::to_string(col, str);
        reverse(str.begin(), str.end());
        cout << str << endl;
    }

    cout << "\nHeader" << endl;
    for (size_t i = 0; i < header.size(); ++i) {
        cout << header[i] << " \n"[i == header.size() - 1];
    }
}

string BaseSolver::coverage2String(const dynamic_bitset<unsigned char>& coverage)
{
    string str;
    size_t j = coverage.find_first();
    if (j == coverage.npos) return str;
    str += to_string(j);
    while ((j = coverage.find_next(j)) != coverage.npos) {
        str += "," + to_string(j);
    }
    return str;
}