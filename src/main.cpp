#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>

#include "Greedy.hpp"
#include "ECGS.hpp"
#include "RECGS.hpp"

using namespace std;
using namespace std::chrono;


int main()
{
    filesystem::path matrix_path   = "../csv/bool_matrix.csv",
                     coverage_path = "/home/srg/Documents/git/LAD/src/results.txt",
                     time_path     = "/home/srg/Documents/git/LAD/src/time.txt";

    Greedy solver(matrix_path);
    // RECGS solver(matrix_path);
    // ECGS solver(matrix_path);

    auto start = high_resolution_clock::now();
    dynamic_bitset<unsigned char> coverage = solver.solve();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Coverage " << solver.coverage2String(coverage) << endl;
    cout << "Working time " << duration.count() << " ms" << endl;
    cout << "Max number of bins " << solver.maxBins(coverage) << endl;

    /* Write coverage to file */
    ofstream coverage_file(coverage_path, ofstream::out);
    coverage_file << solver.coverage2String(coverage) << endl;
    coverage_file.close();

    /* Save running time */
    if (filesystem::exists(time_path)) {
        ofstream time_file(time_path, ofstream::app);
        time_file << ',' << duration.count();
        time_file.close();
    } else {
        ofstream time_file(time_path, ofstream::out);
        time_file << duration.count();
        time_file.close();
    }
}