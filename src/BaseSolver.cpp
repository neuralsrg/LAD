#include "BaseSolver.hpp"

/* Constructor */

BaseSolver::BaseSolver(string filename) : mat(), header(0)
{
    parseCsv(filename);
    buildColMat();
}

/* Private members */

void BaseSolver::parseCsv(string filename)
{
    string row;
    ifstream boolfile;
    int number, i = 0;

    /* Openning file containing boolean matrix */
    boolfile.open(filename);
    while(getline(boolfile, row))
    {
        if (i++ == 0) {
            replace(row.begin(), row.end(), ',', ' ');
            stringstream ss(row);
            while (ss >> number)
                header.push_back(number);
        } else {
            row.erase(remove(row.begin(), row.end(), ','), row.end());
            reverse(row.begin(), row.end()); // bitset[0] -> first column
            mat.row_mat.push_back(dynamic_bitset<unsigned char>(row));
        }
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

/* Protected members */

bool BaseSolver::checkCoverage(const dynamic_bitset<unsigned char>& coverage) const
{
    for (const auto& row : mat.row_mat) {
        if ((row & coverage).count() == 0) {
            return false;
        } 
    }
    return true;
}

/* Public members */

void BaseSolver::printMatrix() const
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
    cout << endl;
}

string BaseSolver::coverage2String(const dynamic_bitset<unsigned char>& coverage) const
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

int BaseSolver::maxBins(const dynamic_bitset<unsigned char>& coverage) const
{
    int n = 0, max_n = 0, prev = 0;
    size_t j = coverage.find_first();
    do {
        if (prev == header[j]) {
            ++n;
        } else {
            max_n = n > max_n ? n : max_n;
            n = 1;
        }
        prev = header[j];
        j = coverage.find_next(j);
    } while (j != coverage.npos);
    return max_n + 1;
}