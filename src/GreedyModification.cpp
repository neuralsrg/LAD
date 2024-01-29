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
    int i = header[include], e = header[exclude[0]];
    if (intervals[e] - exclude.size() >= intervals[i] + 1) {
        intervals[e] -= exclude.size();
        intervals[i]++;
        return true;
    }
    return false;
}

// CHANGE TO VECTOR<SIZE_T>
dynamic_bitset<unsigned char> GreedyModification::getLargestGroup(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals) const
{
    dynamic_bitset<unsigned char> group(header.size(), 0);
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
            group.set(curCol);
        }
        curCol = coverage.find_next(curCol);
    } while (header[curCol] <= j);
    return group;
}

void GreedyModification::removeHeaviestCol(dynamic_bitset<unsigned char>& group, map<int, vector<size_t>> supportRows) const
{
    size_t freq = 0, curCol = group.find_first(), j = 0;
    do {
        if (supportRows[curCol].size() > freq) {
            j = curCol;
            freq = supportRows[curCol].size();
        }
        curCol = group.find_next(curCol);
    } while (curCol != group.npos);
    group.reset(j);
}

vector<size_t> GreedyModification::getLightestCols(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals, map<int, vector<size_t>> supportRows) const
{
    vector<size_t> cols;
    dynamic_bitset<unsigned char> group = getLargestGroup(coverage, intervals);
    size_t min_support = mat.row_mat.size(), j = group.find_first();
    do {
        if (min_support > supportRows[j].size()) {
            min_support = supportRows[j].size();
            cols.clear();
        }
        if (min_support == supportRows[j].size()) {
            cols.push_back(j);
        }
        j = group.find_next(j);
    } while (j != group.npos);
    return cols;
}

map<int, vector<size_t>> GreedyModification::getSupportRows(const dynamic_bitset<unsigned char>& coverage) const
{
    map<int, vector<size_t>> dict;
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

vector<size_t> GreedyModification::getCoveringCols(const vector<size_t>& cols, const map<int, vector<size_t>>& supportRows) const
{
    dynamic_bitset<unsigned char> testRow(mat.col_mat.size(), 0);
    testRow.set();
    for (const auto& p : supportRows) {
        for (const auto& r : p.second) {
            testRow &= mat.row_mat[r];
        }
    }
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
    map<int, vector<size_t>> supportRows = getSupportRows(coverage);

    cout << "Support rows:" << endl;
    for (const auto& r : supportRows) {
        cout << "Column " << r.first << ": ";
        for (auto it = r.second.begin(); it != r.second.end(); it++) {
            cout << *it << ' ';
        }
        cout << endl;
    }

    // cout << "Intervals:" << endl;
    // for (auto p : intervals) {
    //     cout << p.first << ": " << p.second << endl;
    // }

    vector<size_t> cols = getLightestCols(coverage, intervals, supportRows);
    for (auto v : cols) {
        cout << v << ' ';
    }
    cout << endl;

    return coverage;
}

int main()
{
    cout << dynamic_bitset<unsigned char>(5, 0).set() << endl;
    GreedyModification solver("../csv/bool_matrix.csv");

    auto start = high_resolution_clock::now();

    dynamic_bitset<unsigned char> coverage = solver.solve();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Coverage " << solver.coverage2String(coverage) << endl;

    cout << "Working time " << duration.count() << " ms" << endl;

    return 0;
}
