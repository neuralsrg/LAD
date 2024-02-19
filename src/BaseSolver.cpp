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

void BaseSolver::reduceCoverage(dynamic_bitset<unsigned char>& coverage)
{
    vector<int> count_features(header.back() + 1, 0);
    size_t j = coverage.find_first();
    do {
        count_features[header[j]]++;
    }
    while ((j = coverage.find_next(j)) != coverage.npos);

    j = coverage.find_first();
    do {
        if (count_features[header[j]] <= 1) continue;
        coverage.reset(j);
        if (checkCoverage(coverage)) {
            count_features[header[j]]--;
        } else {
            coverage.set(j);
        }
    }
    while ((j = coverage.find_next(j)) != coverage.npos);
}

void BaseSolver::addColumn(vector<int>& covers_uncovered, dynamic_bitset<unsigned char>& coverage, set<int>& uncovered_rows, int newColumn)
{
    const auto& column = mat.col_mat[newColumn];
    size_t coveredRow = column.find_first();
    if (coveredRow == column.npos) throw("Empty column!");
    do {
        auto it = uncovered_rows.find(coveredRow);
        if (it == uncovered_rows.end()) continue;
        uncovered_rows.erase(it);
        const auto& row = mat.row_mat[coveredRow];
        size_t j = row.find_first();
        if (j != row.npos) {
            do {
                if (covers_uncovered[j] > 0) {
                    covers_uncovered[j]--;
                } else if (covers_uncovered[j] < 0) {
                    covers_uncovered[j]++;
                } else throw("covers_uncovered[j] = 0");
            }
            while ((j = row.find_next(j)) != row.npos);
        }
    }
    while ((coveredRow = column.find_next(coveredRow)) != column.npos);
    coverage.set(newColumn);
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