#include <chrono>

#include "GreedyModification.hpp"

using namespace std::chrono;


/* Protected members */

map<int, size_t> GreedyModification::getNumIntervals(const dynamic_bitset<unsigned char>& coverage) const
{
    map<int, size_t> dict;
    size_t j = coverage.find_first();
    do {
        if (dict.find(header[j]) == dict.end()) {
            dict.insert({header[j], 1});
        } else {
            dict[header[j]] += 1;
        }
        j = coverage.find_next(j);
    } while (j != coverage.npos);
    return dict;
}

bool GreedyModification::gain(const vector<int>& exclude, int include, const dynamic_bitset<unsigned char>& coverage, map<int, size_t>& intervals) const
{
    if ((exclude.size() == 1) && (header[exclude[0]] == header[include])) return false;
    int i = header[include], e = header[exclude[0]];
    if (intervals[e] - exclude.size() >= intervals[i] + 1) {
        intervals[e] -= exclude.size();
        intervals[i]++;
        return true;
    }
    return false;
}

vector<size_t> GreedyModification::getLargestGroup(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals) const
{
    vector<size_t> group;
    int j = 0;
    size_t freq = 0, curCol = coverage.find_first();
    for (const auto& p : intervals) {
        if (p.second > freq) {
            j = p.first;
            freq = p.second;
        }
    }
    do {
        if (header[curCol] == j) {
            group.push_back(curCol);
        }
        curCol = coverage.find_next(curCol);
    } while (header[curCol] <= j);
    return group;
}

void GreedyModification::removeHeaviestCol(vector<size_t>& group, map<size_t, vector<size_t>> supportRows) const
{
    size_t freq = 0, j = 0;
    for (size_t i = 0; i < group.size(); ++i) {
        size_t curCol = group[i];
        if (supportRows[curCol].size() > freq) {
            j = i;
            freq = supportRows[curCol].size();
        }
    }
    group.erase(group.begin() + j);
}

vector<size_t> GreedyModification::getLightestCols(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals, map<size_t, vector<size_t>> supportRows) const
{
    vector<size_t> cols, group = getLargestGroup(coverage, intervals);
    size_t min_support = mat.row_mat.size();
    for (const auto& j : group) {
        if (min_support > supportRows[j].size()) {
            min_support = supportRows[j].size();
            cols.clear();
        }
        if (min_support == supportRows[j].size()) {
            cols.push_back(j);
        }
    }
    return cols;
}

map<size_t, vector<size_t>> GreedyModification::getSupportRows(const dynamic_bitset<unsigned char>& coverage) const
{
    map<size_t, vector<size_t>> dict;
    for (size_t i = 0; i < mat.row_mat.size(); ++i) {
        if ((mat.row_mat[i] & coverage).count() != 1) {
            continue;
        }
        size_t j = (mat.row_mat[i] & coverage).find_first();
        if (dict.find(j) == dict.end()) {
            dict.insert({j, vector<size_t> {i}});
        } else {
            dict[j].push_back(i);
        }
    }
    return dict;
}

vector<size_t> GreedyModification::getCoveringCols(const vector<size_t>& cols, const map<size_t, vector<size_t>>& supportRows) const
{
    dynamic_bitset<unsigned char> testRow(mat.col_mat.size(), 0);
    testRow.set();
    for (const auto& p : cols) {
        for (const auto& r : supportRows.at(p)) {
            testRow &= mat.row_mat[r];
        }
    }
    cout << "Num covering cols: " << testRow.count() << endl;
    if (testRow.none()) return {};
    vector<size_t> coveringCols;
    size_t j = testRow.find_first();
    do {
        coveringCols.push_back(j);
        j = testRow.find_next(j);
    } while (j != testRow.npos);
    return coveringCols;
}

/* Public members */

GreedyModification::GreedyModification(string filename) : GreedySolver(filename) { }

dynamic_bitset<unsigned char> GreedyModification::solve()
{
    dynamic_bitset<unsigned char> coverage = GreedySolver::solve();
    map<int, size_t> intervals = getNumIntervals(coverage);
    map<size_t, vector<size_t>> supportRows = getSupportRows(coverage);

    // cout << "Support rows:" << endl;
    // for (const auto& r : supportRows) {
    //     cout << "Column " << r.first << ": ";
    //     for (auto it = r.second.begin(); it != r.second.end(); it++) {
    //         cout << *it << ' ';
    //     }
    //     cout << endl;
    // }

    // cout << "Intervals:" << endl;
    // for (auto p : intervals) {
    //     cout << p.first << ": " << p.second << endl;
    // }

    // cout << "Lightest group" << endl;
    // vector<size_t> group = getLargestGroup(coverage, intervals);
    // for (auto v : group) {
    //     cout << v << ' ';
    // }
    // cout << endl;

    // cout << "Lightest columns" << endl;
    // vector<size_t> cols = getLightestCols(coverage, intervals, supportRows);
    // for (auto v : cols) {
    //     cout << v << ' ';
    // }
    // cout << endl;

    cout << "Covering Columns" << endl;
    vector<size_t> coveringCols = getCoveringCols(vector<size_t> {105, 237}, supportRows);
    for (auto v : coveringCols) {
        cout << v << ' ';
    }
    cout << endl;

    return coverage;
}

int main()
{
    GreedyModification solver("../csv/bool_matrix.csv");

    auto start = high_resolution_clock::now();

    dynamic_bitset<unsigned char> coverage = solver.solve();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Coverage " << solver.coverage2String(coverage) << endl;

    cout << "Working time " << duration.count() << " ms" << endl;

    return 0;
}
