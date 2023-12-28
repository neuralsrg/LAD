#include "GreedySolver.hpp"

/* Private members */

void GreedySolver::addColumn(vector<int>& covers_uncovered, dynamic_bitset<unsigned char>& coverage, set<int>& uncovered_rows, int newColumn)
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

void GreedySolver::reduceCoverage(dynamic_bitset<unsigned char>& coverage)
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
        cout << "Trying to kick column " << j << endl;
        if (checkCoverage(coverage)) {
            count_features[header[j]]--;
        } else {
            coverage.set(j);
        }
    }
    while ((j = coverage.find_next(j)) != coverage.npos);
}

/* Public members */

GreedySolver::GreedySolver(string filename) : BaseSolver(filename) { }

dynamic_bitset<unsigned char> GreedySolver::solve()
{
    vector<int> covers_uncovered(header.size(), 0);
    dynamic_bitset<unsigned char> coverage(header.size(), 0);
    set<int> uncovered_rows(counting_iterator<int>(0), counting_iterator<int>(mat.row_mat.size()));
    set<int> uncovered_features(counting_iterator<int>(0), counting_iterator<int>(header.back() + 1));

    for (size_t j = 0; j < header.size(); ++j) {
        covers_uncovered[j] = mat.col_mat[j].count();
    }

    while (!uncovered_rows.empty()) {
        while (true) {
            // if (uncovered_rows.empty()) break;
            auto it = max_element(covers_uncovered.begin(),
                                  covers_uncovered.end());
            if (*it <= 0) break;
            int j = distance(covers_uncovered.begin(), it);
            addColumn(covers_uncovered, coverage, uncovered_rows, j);
            uncovered_features.erase(header[j]);
            for (size_t t = 0; t < covers_uncovered.size(); ++t) {
                if (header[t] == header[j]) covers_uncovered[t] *= -1;
                else if (header[t] > header[j]) break;
            }
        }
        for (size_t t = 0; t < covers_uncovered.size(); ++t) {
            covers_uncovered[t] *= -1;
        }
    }
    if (!uncovered_features.empty()) {
        for (size_t t = 0; t < header.size(); ++t) {
            if (uncovered_features.find(header[t]) != uncovered_features.end()) {
                uncovered_features.erase(header[t]);
                coverage.set(t);
            }
        }
    }
    reduceCoverage(coverage);
    return coverage;
}


int main()
{
    GreedySolver solver("../csv/bool_matrix.csv");
    dynamic_bitset<unsigned char> coverage = solver.solve();

    cout << "Coverage " << solver.coverage2String(coverage) << endl;
    
    return 0;
}