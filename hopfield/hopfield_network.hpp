#ifndef HOPFIELD_DATA
#define HOPFIELD_DATA

#include <vector>

class HopfieldNetwork {
public:
    HopfieldNetwork(int rows, int cols)
        : mRows(rows), mCols(cols), mMatrix(rows * cols, -1) {}

    int rows() const { return mRows; }
    int cols() const { return mCols; }

    int& at(int row, int col) { return mMatrix[row * mCols + col]; }
    int at(int row, int col) const { return mMatrix[row * mCols + col]; }

    std::vector<int>& data() { return mMatrix; }
    const std::vector<int>& data() const { return mMatrix; }

private:
    int mRows, mCols;
    std::vector<int> mMatrix; // -1 or 1
};

#endif // HOPFIELD_DATA