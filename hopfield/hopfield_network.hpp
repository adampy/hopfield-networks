#ifndef HOPFIELD_NETWORK
#define HOPFIELD_NETWORK

#include <vector>
#include <string>

class HopfieldNetwork {
public:
    using Memory = std::vector<int>;

    // Constructor with no data
    HopfieldNetwork(int rows, int cols)
        : mRows(rows), mCols(cols), mMatrix(rows * cols, -1) {}

    // Constructor with memories
    HopfieldNetwork(int rows, int cols, const std::vector<Memory>& memories)
        : mRows(rows), mCols(cols), mMatrix(rows * cols, -1), mMemories(memories) {}

    static HopfieldNetwork fromFile(const std::string& filename);

    void saveToFile(const std::string& filename) const;

    int rows() const { return mRows; }
    int cols() const { return mCols; }

    int& at(int row, int col) { return mMatrix[row * mCols + col]; }
    int at(int row, int col) const { return mMatrix[row * mCols + col]; }

    std::vector<int>& data() { return mMatrix; }
    const std::vector<int>& data() const { return mMatrix; }

    const std::vector<std::vector<int>>& getMemories() const { return mMemories; }

    void addMemory(const std::vector<int>& memory) { mMemories.push_back(memory); }

    void setWeights(const std::vector<int>& weights) { mMatrix = weights; }

private:
    int mRows, mCols;
    std::vector<int> mMatrix; // -1 or 1
    std::vector<std::vector<int>> mMemories;
};

#endif // HOPFIELD_NETWORK