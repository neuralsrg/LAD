#include <chrono>

#include "GreedyModification.hpp"

using namespace std::chrono;

/* Private members */

void GreedyModification::print(string message, vector<size_t> nums) const
{
    cout << message << ' ';
    for (const auto& v : nums) cout << v << ',';
    cout << endl;
}

void GreedyModification::updateSupportRows(const vector<size_t>& exclude, size_t include, map<size_t, vector<size_t>>& supportRows) const
{
    supportRows.insert({include, vector<size_t>()});
    for (const auto& v : exclude) {
        supportRows[include].insert(supportRows[include].end(), supportRows[v].begin(), supportRows[v].end());
        supportRows.erase(supportRows.find(v));
    }
    sort(supportRows[include].begin(), supportRows[include].end());
}

void GreedyModification::reduceCoverage(dynamic_bitset<unsigned char>& coverage, const size_t& include, map<size_t, vector<size_t>>& supportRows, map<int, size_t>& intervals) const
{
    const dynamic_bitset<unsigned char>& includedCol = mat.col_mat[include];
    for (auto& p: supportRows) {
        if (p.first == include) continue;
        vector<size_t> removeIds;
        for (size_t i = 0; i < p.second.size(); ++i) {
            if (!includedCol.test(p.second[i])) continue;
            removeIds.push_back(i);
        }
        if ((removeIds.size() == p.second.size()) && (intervals[header[p.first]] > 1)) {
            /* Remove column from coverage */
            coverage.reset(p.first);
            supportRows.erase(supportRows.find(p.first));
            intervals[header[p.first]]--;
        } else {
            /* Remove support rows from column */
            for (const auto& i : removeIds) {
                p.second.erase(p.second.begin() + i);
            }
        }
    }
}

/* Protected members */

bool GreedyModification::gain(const vector<size_t>& exclude, size_t include, dynamic_bitset<unsigned char>& coverage, map<int, size_t>& intervals, map<size_t, vector<size_t>>& supportRows) const
{
    if ((exclude.size() == 1) && (header[exclude[0]] == header[include])) return false;
    int i = header[include], e = header[exclude[0]];
    if (intervals[e] > intervals[i]) {
        /* Update Intervals */
        intervals[e] -= exclude.size();
        intervals[i]++;

        /* Update Support Rows */
        updateSupportRows(exclude, include, supportRows);

        /* Update Coverage */
        for (const auto& v : exclude) coverage.reset(v);
        coverage.set(i);

        return true;
    }
    return false;
}

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
    map<size_t, vector<size_t>> supportRows = getSupportRows(coverage);
    cout << "Support rows:" << endl;
    for (const auto& r : supportRows) {
        cout << "Column " << r.first << ": ";
        for (auto it = r.second.begin(); it != r.second.end(); it++) {
            cout << *it << ' ';
        }
        cout << endl;
    }

    map<int, size_t> intervals = getNumIntervals(coverage);
    // cout << "Intervals:" << endl;
    // for (auto p : intervals) {
    //     cout << p.first << ": " << p.second << endl;
    // }

    // vector<size_t> group = getLargestGroup(coverage, intervals);
    // print("Largest group", group);

    // vector<size_t> cols = getLightestCols(coverage, intervals, supportRows);
    // print("Lightest columns", cols);

    // vector<size_t> coveringCols = getCoveringCols(vector<size_t> {105, 237}, supportRows);
    // print("Covering columns", coveringCols);

    bool processing = true;
    while (processing) {
        processing = false;
        vector<size_t> group = getLargestGroup(coverage, intervals);

        /* Trying to remove group */

        while (group.size() > 0) {
            print("Processing group:", group);
            vector<size_t> coveringCols = getCoveringCols(group, supportRows);
            sort(coveringCols.begin(), coveringCols.end(), [&](const size_t& a, const size_t& b) { return mat.col_mat[a].count() > mat.col_mat[b].count(); });
            for (const auto& include : coveringCols) {
                if (gain(group, include, coverage, intervals, supportRows)) {
                    processing = true;
                    reduceCoverage(coverage, include, supportRows, intervals);
                    break;
                }
            }
            if (processing) break;
            removeHeaviestCol(group, supportRows);
        }
        if (processing) continue;

        /* If couldn't remove group */

        vector<size_t> lightestCols = getLightestCols(coverage, intervals, supportRows);
        for (const auto& exclude : lightestCols) {
            vector<size_t> coveringCols = getCoveringCols({exclude}, supportRows);
            sort(coveringCols.begin(), coveringCols.end(), [&](const size_t& a, const size_t& b) { return mat.col_mat[a].count() > mat.col_mat[b].count(); });
            for (const auto& include : coveringCols) {
                if (gain({exclude}, include, coverage, intervals, supportRows)) {
                    processing = true;
                    reduceCoverage(coverage, include, supportRows, intervals);
                    break;
                }
            }
            if (processing) break;
        }
    }
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
