#include "RECGS.hpp"

/* Private members */

void RECGS::print(string message, vector<size_t> nums) const
{
    cout << message << ' ';
    for (const auto& v : nums) cout << v << ',';
    cout << endl;
}

void RECGS::reduceCoverage(dynamic_bitset<unsigned char>& coverage, const size_t& include, map<size_t, vector<size_t>>& supportRows, map<int, size_t>& intervals) const
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
            reverse(removeIds.begin(), removeIds.end());
            for (const auto& i : removeIds) {
                p.second.erase(p.second.begin() + i);
            }
        }
    }
}

/* Protected members */

void RECGS::updateSupportRows(const vector<size_t>& exclude, size_t include, map<size_t, vector<size_t>>& supportRows) const
{
    supportRows.insert({include, vector<size_t>()});
    for (const auto& v : exclude) {
        supportRows[include].insert(supportRows[include].end(), supportRows[v].begin(), supportRows[v].end());
        supportRows.erase(supportRows.find(v));
    }
    sort(supportRows[include].begin(), supportRows[include].end());
}

bool RECGS::gain(const vector<size_t>& exclude, size_t include, dynamic_bitset<unsigned char>& coverage, map<int, size_t>& intervals, map<size_t, vector<size_t>>& supportRows) const
{
    if ((exclude.size() == 1) && (header[exclude[0]] == header[include])) return false;
    int i = header[include], e = header[exclude[0]];
    if (intervals[e] > intervals[i] + 1 && intervals[e] > exclude.size()) {
        // cout << "GAIN: intervals[e] = " << intervals[e] << "; intervals[i] = " << intervals[i] << endl;
        /* Update Intervals */
        intervals[e] -= exclude.size();
        intervals[i]++;

        /* Update Support Rows */
        updateSupportRows(exclude, include, supportRows);

        /* Update Coverage */
        for (const auto& v : exclude) coverage.reset(v);
        coverage.set(include);

        return true;
    }
    return false;
}

map<int, size_t> RECGS::getNumIntervals(const dynamic_bitset<unsigned char>& coverage) const
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

vector<vector<size_t>> RECGS::getLargestGroups(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals) const
{
    vector<vector<size_t>> groups;
    size_t freq = 0;
    for (const auto& p : intervals) {
        if (p.second > freq) freq = p.second;
    }
    vector<int> featuresWithMaxFreq;
    for (const auto& p : intervals) {
        if (p.second == freq) featuresWithMaxFreq.push_back(p.first);
    }
    for (const auto& j : featuresWithMaxFreq) {
        size_t curCol = coverage.find_first();
        vector<size_t> group;
        do {
            if (header[curCol] == j) {
                group.push_back(curCol);
            }
            curCol = coverage.find_next(curCol);
        } while (header[curCol] <= j && curCol != coverage.npos);
        groups.push_back(group);
    }
    return groups;
}

void RECGS::removeHeaviestCol(vector<size_t>& group, map<size_t, vector<size_t>> supportRows) const
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

// vector<vector<size_t>> RECGS::getLightestCols(const dynamic_bitset<unsigned char>& coverage, const map<int, size_t>& intervals, map<size_t, vector<size_t>> supportRows) const
// {
//     vector<vector<size_t>> cols;
//     vector<vector<size_t>> groups = getLargestGroups(coverage, intervals);

//     for (const auto& group : groups) {
//         size_t min_support = mat.row_mat.size();
//         vector<size_t> col;
//         for (const auto& j : group) {
//             if (min_support > supportRows[j].size()) {
//                 min_support = supportRows[j].size();
//                 col.clear();
//             }
//             if (min_support == supportRows[j].size()) {
//                 col.push_back(j);
//             }
//         }
//         cols.push_back(col);
//     }
//     return cols;
// }

map<size_t, vector<size_t>> RECGS::getSupportRows(const dynamic_bitset<unsigned char>& coverage) const
{
    map<size_t, vector<size_t>> dict;
    size_t j = coverage.find_first();
    do {
        dict.insert({j, {}});
        j = coverage.find_next(j);
    } while (j != coverage.npos);

    for (size_t i = 0; i < mat.row_mat.size(); ++i) {
        if ((mat.row_mat[i] & coverage).count() != 1) {
            continue;
        }
        size_t j = (mat.row_mat[i] & coverage).find_first();
        dict[j].push_back(i);
    }
    return dict;
}

vector<size_t> RECGS::getCoveringCols(const vector<size_t>& cols, const map<size_t, vector<size_t>>& supportRows) const
{
    dynamic_bitset<unsigned char> testRow(mat.col_mat.size(), 0);
    testRow.set();
    for (const auto& p : cols) {
        testRow.reset(p);
        for (const auto& r : supportRows.at(p)) {
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

RECGS::RECGS(string filename) : ECGS(filename) { }

dynamic_bitset<unsigned char> RECGS::solve()
{
    dynamic_bitset<unsigned char> coverage = ECGS::solve();
    map<size_t, vector<size_t>> supportRows = getSupportRows(coverage);
    // cout << "Support rows:" << endl;
    // for (const auto& r : supportRows) {
    //     cout << '[' << header[r.first] << ']' << " Column " << r.first << ": ";
    //     for (auto it = r.second.begin(); it != r.second.end(); it++) {
    //         cout << *it << ' ';
    //     }
    //     cout << endl;
    // }

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

    bool modified = true;
    while (modified) {
        // cout << "\nCurrent coverage: " << coverage2String(coverage) << '\n' << endl;
        modified = false;
        vector<vector<size_t>> groups = getLargestGroups(coverage, intervals);

        /* Trying to remove group */
        for (auto& group : groups) {
            while (group.size() > 0) {
                // print("\nProcessing group:", group);
                // if (group.size() > 0) cout << "Which belongs to feature " << header[group[0]] << endl;
                vector<size_t> coveringCols = getCoveringCols(group, supportRows);
                sort(coveringCols.begin(), coveringCols.end(), [&](const size_t& a, const size_t& b) { return mat.col_mat[a].count() > mat.col_mat[b].count(); });
                // print("Sorted covering cols:", coveringCols);
                for (const auto& include : coveringCols) {
                    if (gain(group, include, coverage, intervals, supportRows)) {
                        modified = true;
                        // cout << "\nUsed " << include << " column to update coverage\n" << endl;
                        reduceCoverage(coverage, include, supportRows, intervals);
                        break;
                    }
                }
                if (modified) break;
                removeHeaviestCol(group, supportRows);
                // cout << "Heaviest column removed" << endl;
            }
            if (modified) break;
        }
        if (modified) continue;

        /* If couldn't remove group */

        // cout << "\nTrying to remove columns" << endl;

        // vector<vector<size_t>> allLightestCols = getLightestCols(coverage, intervals, supportRows);
        groups = getLargestGroups(coverage, intervals);
        for (const auto& group : groups) {
            // print("\nWill try to remove one of", group);
            for (const auto& exclude : group) {
                // cout << "Processing " << exclude << " column" << endl;
                vector<size_t> coveringCols = getCoveringCols({exclude}, supportRows);
                // print("It can be replaced with one of", coveringCols);
                sort(coveringCols.begin(), coveringCols.end(), [&](const size_t& a, const size_t& b) { return mat.col_mat[a].count() > mat.col_mat[b].count(); });
                for (const auto& include : coveringCols) {
                    if (gain({exclude}, include, coverage, intervals, supportRows)) {
                        modified = true;
                        reduceCoverage(coverage, include, supportRows, intervals);
                        // cout << "And we replace it with " << include << " column" << endl;
                        break;
                    }
                }
                if (modified) break;
            }
            if (modified) break;
        }
        // cout << "Support rows:" << endl;
        // for (const auto& r : supportRows) {
        //     cout << "Column " << r.first << ": ";
        //     for (auto it = r.second.begin(); it != r.second.end(); it++) {
        //         cout << *it << ' ';
        //     }
        //     cout << endl;
        // }
    }
    return coverage;
}