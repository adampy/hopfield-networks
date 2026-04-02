#ifndef HOPFIELD_NETWORK
#define HOPFIELD_NETWORK

#include <vector>
#include <string>
#include "hopfield/IHopfieldNetwork.hpp"

/**
 * Representation of a classical Hopfield network with weight matrix
 * and associated memories.
 
 * @author Adam Crawley
 */
class HopfieldNetwork : public IHopfieldNetwork {
public:
    // Constructor with no data
    HopfieldNetwork(int rows, int cols)
        : mRows(rows), mCols(cols), mMatrix(rows * cols, -1) {}

    // Constructor with memories
    HopfieldNetwork(int rows, int cols, const std::vector<std::vector<int>>& memories)
        : mRows(rows), mCols(cols), mMatrix(rows * cols, -1), mMemories(memories) {}

    /**
     * Static constructor that reads a network architecture (rows, cols) and
     * memories from a file.
     * @note Memories are stored but network weights are not changed YET!
     */
    static HopfieldNetwork fromFile(const std::string& filename);

    void saveToFile(const std::string& filename) const override;

    int rows() const override { return mRows; }
    int cols() const override { return mCols; }

    int& at(int row, int col) override { return mMatrix[row * mCols + col]; }
    int at(int row, int col) const override { return mMatrix[row * mCols + col]; }

    std::vector<int>& data() override { return mMatrix; }
    const std::vector<int>& data() const override { return mMatrix; }

    const std::vector<std::vector<int>>& getMemories() const override { return mMemories; }

    void addMemory(const std::vector<int>& memory) override { mMemories.push_back(memory); }

    void setWeights(const std::vector<int>& weights) override { mMatrix = weights; }

private:
    int mRows, mCols;
    std::vector<int> mMatrix; // -1 or 1
    std::vector<std::vector<int>> mMemories;
};

#endif // HOPFIELD_NETWORK