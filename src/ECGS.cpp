#include "ECGS.hpp"

/* Public members */

ECGS::ECGS(string filename) : BaseSolver(filename) { }

dynamic_bitset<unsigned char> ECGS::solve()
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