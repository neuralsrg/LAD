#include "HeuristicOpt.hpp"

/* Private members */

bool HeuristicOpt::checkGain(size_t exclude, size_t include, const map<int, size_t>& intervals) const
{
    return mat.col_mat[exclude].count() > mat.col_mat[include].count() && (
        intervals.at(header[exclude]) > 1 ||
        (intervals.at(header[exclude]) == 1 && header[exclude] == header[include])
        );
}

void HeuristicOpt::updateStatistics(size_t exclude, size_t include,
    map<size_t, vector<size_t>>& supportRows, map<int, size_t>& intervals) const
{
    if (header[exclude] != header[include]) {
        intervals[header[exclude]]--;
        intervals[header[include]]++;
    }
    updateSupportRows({exclude}, include, supportRows);
}

bool HeuristicOpt::checkHeuristicGain(const vector<size_t>& coverageCols, size_t include) const
{
    double h = 0;
    for (const auto& col : coverageCols) {
        h += 1.0 / double(mat.col_mat[col].count());
    }
    return (h + 1.0 / double(mat.col_mat[include].count())) / double(coverageCols.size() + 1) >
        h / double(coverageCols.size() + 1);
}

double HeuristicOpt::computeHeuristic(const vector<size_t>& coverageCols) const
{
    double h = 0;
    for (const auto& col : coverageCols) {
        h += 1.0 / double(mat.col_mat[col].count());
    }
    return h / coverageCols.size();
}


/* Public members */

HeuristicOpt::HeuristicOpt(string filename) : RECGS(filename) { }

dynamic_bitset<unsigned char> HeuristicOpt::solve()
{
    dynamic_bitset<unsigned char> coverage = RECGS::solve();
    map<size_t, vector<size_t>> supportRows = getSupportRows(coverage);
    map<int, size_t> intervals = getNumIntervals(coverage);
    // cout << "Intervals:" << endl;
    // for (auto p : intervals) {
    //     cout << p.first << ": " << p.second << endl;
    // }

    /* Get encoding coverage and sort its columns */
    vector<size_t> coverageCols;
    size_t curCol = coverage.find_first();
    do {
        coverageCols.push_back(curCol);
        curCol = coverage.find_next(curCol);
    } while (curCol != coverage.npos);
    cout << "Heuristic Before " << computeHeuristic(coverageCols) << endl;
    auto sortingFn = [&](size_t i, size_t j)
    {
        return mat.col_mat[i].count() > mat.col_mat[j].count();
    };
    sort(coverageCols.begin(), coverageCols.end(), sortingFn);
    auto inverseSortingFn = [&](size_t i, size_t j)
    {
        return mat.col_mat[i].count() < mat.col_mat[j].count();
    };

    /* Optimization Loop */
    bool modified = true;
    while (modified) {
        modified = false;
        for (size_t col_idx = 0; col_idx < coverageCols.size(); ++col_idx) {
            size_t exclude = coverageCols[col_idx];
            // cout << "\nTrying to exclude " << exclude << endl;
            vector<size_t> covering = getCoveringCols({exclude}, supportRows);
            if (covering.empty()) continue;
            sort(covering.begin(), covering.end(), inverseSortingFn);
            for (const auto& replace : covering) {
                // cout << "Trying to replace it with " << replace << endl;
                if (checkGain(exclude, replace, intervals)) {
                    coverage.reset(exclude);
                    coverage.set(replace);
                    coverageCols[col_idx] = replace;
                    sort(coverageCols.begin(), coverageCols.end(), sortingFn);
                    updateStatistics(exclude, replace, supportRows, intervals);
                    modified = true;
                    // cout << "Replaced column " << exclude << " with " << replace << endl;
                    // cout << mat.col_mat[exclude].count() << " vs " << mat.col_mat[replace].count() << endl;
                    break;
                }
            }
            if (modified) break;
        }
    }

    /* Append Columns */
    vector<size_t> allCols(mat.col_mat.size());
    iota(allCols.begin(), allCols.end(), 0);
    sort(allCols.begin(), allCols.end(), inverseSortingFn);
    for (const auto& col : allCols) {
        if (coverage.test(col)) continue;
        if (checkHeuristicGain(coverageCols, col)) {
            coverage.set(col);
            coverageCols.push_back(col);
            // cout << "Appended column " << col << endl;
            continue;
        }
        break;
    }
    cout << "Heuristic After " << computeHeuristic(coverageCols) << endl;
    return coverage;
}