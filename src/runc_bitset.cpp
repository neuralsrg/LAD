#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <time.h>
#include <string>
#include <iomanip>
#include <chrono>
#include <bitset>
#include <algorithm>
#include <random>
#include <filesystem>
#include <map>
#include <cassert>

#include <chrono>
using namespace std::chrono;

#include "../csv/parse_csv.hpp"

std::mt19937 generator;

using namespace std;

namespace fs = filesystem;

const int m = params::m, n = params::n;
const int full_m = params::full_m, full_n = params::full_n;
using TBitRow = params::TBitRow;
using TBitCol = params::TBitCol;
using TMatrix = params::TMatrix;

bool random_bool_with_prob( double prob )
{
    std::bernoulli_distribution d(prob);
    return d(generator);
}

void Print(const vector<TBitRow>& ans) {
    for (size_t idx = 0; idx < ans.size(); ++idx) {
        for (int i = 0; i < n; ++i) {
            if (ans[idx][i]) {
                cout << i << ' ';
            }
        }
        cout << endl;
    }
}
class ISolver {
protected:
    int M, N;
    struct TContext {
        TContext()
            : S(n)
            {}

        TBitCol R;
        TBitRow D, H;
        vector<TBitCol> S;
    } Context;

    vector<size_t> Count;
    TMatrix Mat;

public:
    ISolver(TMatrix mat)
        : M(m)
        , N(n)
        , Context()
        , Mat(mat)
    {
        for (size_t i = 0; i < M; ++i) {
            Context.R.set(i);
            Count.push_back(Mat.row_mat[i].count());
        }
        for (size_t j = 0; j < N; ++j) {
            Context.D.set(j);
        }
    }

    virtual vector<TBitRow> solve() = 0;
};

class SmartSolver : public ISolver {
    void FillS(size_t j) {
        Context.S[j] = Context.R & Mat.col_mat[j];
    }

    size_t GetWithMinWeight() {
        ssize_t minIdx = -1;
        for (size_t idx = 0; idx < m; ++idx) {
            if (Context.R[idx]) {
                if (minIdx == -1) {
                    minIdx = idx;
                } else if (Count[idx] < Count[minIdx]) {
                    minIdx = idx;
                }
            }
        }
        return minIdx;
    }
    
    void BuildSubtreeRUNC(vector<TBitRow>& ans) {
        size_t i = GetWithMinWeight();
        for (size_t colIdx = 0; colIdx < n; ++colIdx) {
            if (!(Context.D[colIdx] && Mat.row_mat[i][colIdx])) {
                continue;
            }
            Context.D.reset(colIdx);
            TContext savedContext(Context);
            CreateNodeRUNC(colIdx);
            if (Context.R.count() == 0) {
                ans.emplace_back(std::move(Context.H));
            } else {
                BuildSubtreeRUNC(ans);
            }
            Context = std::move(savedContext);
        }
    }

    void CreateNodeRUNC(size_t j) {
        FillS(j);
        
        Context.R ^= Context.S[j]; // ~(R && MatCol[j])
        
        Context.H.set(j);

        if (Context.R.count() == 0) {
            return;
        }
        for (size_t coveredCol = 0; coveredCol < n; ++coveredCol) {
            if (!Context.H[coveredCol]) {
                continue;
            }
            if (Context.S[coveredCol].count() == 0) {
                continue;
            }
            if (coveredCol != j) {
                Context.S[coveredCol] &= ~Mat.col_mat[j];
            }

            //maybe faster with this
            /*bool needContinue = false;
            
            for (size_t row = 0; row < m; ++row) {
                if (Context.S[coveredCol][row] &&
                    ((Mat.row_mat[row] & Context.D).count() == 0)) {
                    Context.S[coveredCol] = 0;
                    needContinue = true;
                    break;
                }
            }
            if (needContinue) {
                continue;
            }*/
            if (Context.D.count() == 0) {
                return;
            }
            for (size_t colIdx = 0; colIdx < n; ++colIdx) {
                if (!Context.D[colIdx]) {
                    continue;
                }
                if ((Context.S[coveredCol] & Mat.col_mat[colIdx]) == Context.S[coveredCol]) {
                    Context.D.reset(colIdx);
                }
            }
        }
    }

public:
    SmartSolver(TMatrix str) : ISolver(str) {}

    vector<TBitRow> solve() override {
        vector<TBitRow> ans;
        BuildSubtreeRUNC(ans);
        return ans;
    }
};

class StupidSolver : public ISolver {
private:

    bool CheckCovered(long long mask) {
        vector<bool> checkRow(M);
        size_t colIdx = 0;
        while (mask) {
            if (mask & 1) {
                for (size_t rowIdx = 0; rowIdx < M; ++rowIdx) {
                    checkRow[rowIdx] = checkRow[rowIdx] || Mat.row_mat[rowIdx][colIdx];
                }
            }
            mask >>= 1;
            ++colIdx;
        }
        return std::all_of(checkRow.begin(), checkRow.end(), [](const auto& check) {return check == true;});
    }

    vector<long long> GenerateSubmasks(long long mask) {
        vector<long long> submasks;
        for (long long bit = 1; bit <= mask; bit <<= 1) {
            if (mask & bit) {
                submasks.push_back(mask ^ bit);
            }
        }
        return submasks;
    }

    TBitRow MakeCoverage(long long mask) {
        TBitRow s;
        size_t idx = 0;
        while (mask) {
            if (mask & 1) {
                s.set(idx);
            }
            mask >>= 1;
            ++idx;
        }
        return s;
    }

public:
    StupidSolver(TMatrix mat) : ISolver(mat) {}

    vector<TBitRow> solve() override {
        vector<TBitRow> ans;
        for (int mask = 1; mask < (1 << N); ++mask) {
            if (!CheckCovered(mask)) {
                continue;
            }
            auto submasks = GenerateSubmasks(mask);
            if (!std::none_of(submasks.begin(), submasks.end(), [this](const long long submask) {return CheckCovered(submask);})) {
                continue;
            }
            ans.push_back(MakeCoverage(mask));
        }
        return ans;
    }
};

vector<TMatrix> GenerateInput(float prob) {
    int count = 5;

    vector<TMatrix> res;
    while(count--) {
        bool bad = false;
        vector<std::bitset<n>> row_mat(m);
        vector<std::bitset<m>> col_mat(n);
        for (int i = 0; i < m; ++i) {
            row_mat[i] = std::bitset<n>(0);
        }
        for (int j = 0; j < n; ++j) {
            col_mat[j] = std::bitset<m>(0);
        }
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (random_bool_with_prob(prob)) {
                    row_mat[i].set(j);
                    col_mat[j].set(i);
                }
            }
            if (row_mat[i].count() == 0) {
                bad = true;
            }
        }
        if (bad) {
            ++count;
            continue;
        }
        TMatrix now = {row_mat, col_mat};
        res.push_back(std::move(now));
        }

    return res;
}

string GenerateStringFromCoverages(const vector<TBitRow>& ans) {
    set<string> all;
    // vector<string> all;
    for (const auto& coverage : ans) {
        if (coverage.count() == 0) {
            continue;
        }
        string coverageStr;
        
        for (size_t x = 0; x < n; ++x) {
            if (coverage[x]) {
                coverageStr += (to_string(x) + " ");
            }
        }
        all.insert(std::move(coverageStr));
        // all.push_back(coverageStr);
    }
    string result;
    for (auto coverageStr : all) {
        result += coverageStr +='\n';
    }
    return result;
}

bool IsCoverage(const TMatrix& mat, const TBitRow& coverage) {
    string str(coverage.to_string());
    reverse(str.begin(), str.end());
    TBitRow reversed_coverage(str);
    for (const TBitRow& row : mat.row_mat) {
        if ((row & reversed_coverage).none()) {
            return false;
        }
    }
    return true;
}

int GetOptimalColumn(const TMatrix& mat, const int feature, const vector<int>& header, const TBitRow& coverage) {
    int optimal_column = -1, optimal_covered_rows = -1;
    set<int> set_bits;
    for (int x = 0; x < n; ++x) {
        if (coverage[x]) {
            set_bits.insert(x);
        }
    }
    for (int column = 0; column < n; ++column) {
        if ((header[column] != feature) || (coverage[column])) {
            continue;
        }
        TBitCol bit_column = mat.col_mat[column];
        for (int bit : set_bits) {
            bit_column &= ~mat.col_mat[bit];
        }
        int new_covered_rows = bit_column.count();
        if (optimal_covered_rows < new_covered_rows) {
            optimal_covered_rows = new_covered_rows;
            optimal_column = column;
        }
    }
    return optimal_column;
}

void ReduceCoverage(TBitRow& coverage, const TMatrix& mat, map<int, int>& tff, const vector<int> header) {
    for (int i = 0; i < n; ++i) {
        if (!coverage[i]) {
            continue;
        }
        if (tff.at(header[i]) == 1) {
            continue;
        }
        coverage.reset(i);
        if (!IsCoverage(mat, coverage)) {
            coverage.set(i);
            continue;
        }
        tff[header[i]]--;
    }
}

void ExpandCoverages(vector<TBitRow>& ans, const vector<int>& header, const TMatrix& mat) {

    int min_length = 1<<10;
    set<int> features;
    for (const auto& f : header) {
        features.insert(f);
    }
    for (auto& coverage : ans) {
        if (coverage.count() >= min_length) {
            coverage.reset();
            continue;
        }
        vector<int> covered_features;  // 0 0 2 2 2 7 8
        for (size_t x = 0; x < n; ++x) {
            if (coverage[x]) {
                covered_features.push_back(header[x]);
            }
        }

        map<int, vector<int>> coverage_map;  // {times_covered: vector of features}
        for (auto& f : features) {
            // coverage_map[f] = count(covered_features.begin(), covered_features.end(), f); 
            int times = count(covered_features.begin(), covered_features.end(), f);
            if (coverage_map.find(times) == coverage_map.end()) {
                // not found
                coverage_map[times] = vector<int> { f };
            } else {
                // found
                coverage_map[times].push_back(f);
            }
        }
        while(!IsCoverage(mat, coverage)) {
            int optimal_column = -1;
            int feature_to_append;
            for (auto it = coverage_map.begin(); it != coverage_map.end(); ++it) {
                int smallest_times = it->first;
                for (int idx = 0; idx < coverage_map[smallest_times].size(); ++idx) {
                    feature_to_append = coverage_map[smallest_times][idx];
                    optimal_column = GetOptimalColumn(mat, feature_to_append, header, coverage);
                    if (optimal_column != -1) {
                        // increase number of occurences of selected feature
                        if (coverage_map.find(smallest_times + 1) == coverage_map.end()) {
                            // not found
                            coverage_map[smallest_times + 1] = vector<int> { feature_to_append };
                        } else {
                            // found
                            coverage_map[smallest_times + 1].push_back(feature_to_append);
                        }
                        coverage_map[smallest_times].erase(coverage_map[smallest_times].begin() + idx);
                        if (coverage_map[smallest_times].size() == 0) {
                            coverage_map.erase(smallest_times);
                        }
                        break;
                    }
                }
                if (optimal_column != -1) {
                    break;
                }
            }
            assert(optimal_column != -1);
            coverage.set(optimal_column);
        }
        // while exists uncovered feature
        while ((coverage_map.find(0) != coverage_map.end()) && (!coverage_map[0].empty())) {
            int uncovered_feature = coverage_map[0][0];
            unsigned new_column = distance(header.begin(), find(header.begin(), header.end(), uncovered_feature));
            assert(new_column >= 0 && new_column < n);
            assert(!coverage[new_column]);
            coverage.set(new_column);
            coverage_map[0].erase(coverage_map[0].begin());
        }
        coverage_map.erase(0);
        map<int, int> times_feature_covered;
        for (auto f : features) {
            times_feature_covered[f] = 0;
        }
        for (int x = 0; x < n; ++x) { 
            if (coverage[x]) {
                times_feature_covered[header[x]]++;
            }
        }
        // Reducing coverage
        ReduceCoverage(coverage, mat, times_feature_covered, header);
        int new_length = coverage.count();
        if (new_length < min_length) {
            min_length = new_length;
        }
    }
}

int main() {

    int n = 5;
    int mu = m;
    string path = "../csv/submatrices/" + to_string(n);

    params::Parser p0(path + "/bool_matrix.csv", path + "/bool_matrixT.csv");
    params::TMatrix full_mat = p0.parseCsv(true);
    // **************************************
    std::ifstream header_file;
    vector<int> header;
    string token;
    header_file.open(path + "/header.csv");
    while (getline(header_file, token, ',')) {
        header.push_back(stoi(token));
    }
    header_file.close();
    // **************************************

    path += "/" + to_string(mu);
    ofstream log_file, time_file;
    log_file.open(path + "/logs.txt");
    time_file.open(path + "/time.csv");
    time_file << "n,mu,time\n";

    cout << "Files opened as: " << path + "/logs.txt" << " & " << path + "/time.csv\n";

    for (int i = 0; i < 10; ++i) {
        params::Parser p(path + "/" + to_string(i) + "0.csv", path + "/" + to_string(i) + "1.csv");
        params::TMatrix mat = p.parseCsv();

        SmartSolver smart(mat);

        auto start = high_resolution_clock::now();
        vector<TBitRow> ans = smart.solve();
        ExpandCoverages(ans, header, full_mat);
        auto stop = high_resolution_clock::now();

        cout << "i = " << i << ": ans.size() = " << ans.size() << endl;
        if (ans.size() == 0) {
            continue;
        }
        log_file << GenerateStringFromCoverages(ans);
        auto duration = duration_cast<milliseconds>(stop - start);
        time_file << to_string(n) << ',' << to_string(mu) << ',' << duration.count() << '\n';
    }

    log_file.close();
    time_file.close();
    return 0;
}