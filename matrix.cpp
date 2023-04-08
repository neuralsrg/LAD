#include "matrix.hpp"

Matrix::Matrix(unsigned m, unsigned n, const std::vector<u_int8_t>& v) :
    m(m), n(n), shortest_cover(n+1), bitset(v.size(), 0), mask(v.size(), 0), covers(0)
{
    if (v.size() != m*n)
        throw("Matrix shape is incompatible with given vector.");

    std::size_t i = 0;
    std::for_each(v.begin(), v.end(), [&](uint8_t val)
        { bitset[i++] = val; }); 
}

std::ostream& operator<<(std::ostream& cout, const Matrix& matrix)
{
    for (unsigned i = 0; i < matrix.m; ++i) {
        for (unsigned j = 0; j < matrix.n; j++) {
            cout << matrix.bitset[i*matrix.n + j] << ' ';
        }
        cout << std::endl;
    }
    return cout;
}

void Matrix::gen_mask_cols(const std::vector<size_t>& inds)
{
    mask.reset();
    for (auto p = inds.begin(); p < inds.end(); ++p) {
        if (*p >= n)
            throw "Column index is out of bounds!";
        for (unsigned i = 0; i < m; ++i)
            mask.set((*p) + i*n);
    }
}

void Matrix::gen_mask_rows(const std::vector<size_t>& inds)
{
    mask.reset();
    for (auto p = inds.begin(); p < inds.end(); ++p) {
        if (*p >= m)
            throw "Row index is out of bounds!";
        for (unsigned i = 0; i < n; ++i)
            mask.set((*p)*n + i);
    }
}

bool Matrix::check_cover(const boost::dynamic_bitset<uint8_t>& masked)
{
    std::vector<size_t> row(1);
    for (unsigned i = 0; i < m; ++i) {
        row[0] = i;
        gen_mask_rows(row);
        if (!masked.intersects(mask))
            return false;
    }
    return true;
}

bool Matrix::check_cols(const std::vector<size_t>& inds)
{
    gen_mask_cols(inds);
    return check_cover(bitset & mask);
}

bool Matrix::check_cols(const boost::dynamic_bitset<uint8_t>& bits)
{
    if (bits.none())
        return false;

    std::vector<size_t> bits_inds = {bits.find_first()};
    size_t i = bits_inds[0];
    while ((i = bits.find_next(i)) != bits.npos)
        bits_inds.push_back(i);
    
    return check_cols(bits_inds);
}

void Matrix::mine_covers()
{
    for (unsigned i = 1; i < std::pow(2, n); ++i) {
        boost::dynamic_bitset<uint8_t> cols(n, i);
        size_t len = cols.count();

        /* if this cover has more columns than the shortest one */
        if (len > shortest_cover)
            continue;

        if (check_cols(cols)) {
            if (len == shortest_cover)
                covers.push_back(cols);
            else {
                shortest_cover = len;
                covers.clear();
                covers.push_back(cols);
            }
        }
    }
}

void Matrix::print_covers()
{
    if (covers.empty()) {
        std::cout << "Matrix has no covers." << std::endl;
        return;
    }

    std::cout << "Matrix has " << int(covers.size()) << " shortest covers." << std::endl;

    for (auto& bits: covers) {
        std::vector<size_t> bits_inds = {bits.find_first()};
        size_t i = bits_inds[0];
        while ((i = bits.find_next(i)) != bits.npos)
            bits_inds.push_back(i);
        
        for (auto& ind: bits_inds)
            std::cout << int(ind) << " ";
        std::cout << std::endl;
    }
}