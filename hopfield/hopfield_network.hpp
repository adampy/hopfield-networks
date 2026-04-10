#ifndef HOPFIELD_NETWORK
#define HOPFIELD_NETWORK

#include <vector>
#include <string>
#include "hopfield/IHopfieldNetwork.hpp"

/**
 * Representation of a classical Hopfield network with activation matrix
 * and associated memories.
 
 * @author Adam Crawley
 */
class HopfieldNetwork : public IHopfieldNetwork {
public:
    // Constructor with no data
    HopfieldNetwork(int rows, int cols)
        : mRows(rows), mCols(cols), mSize(rows * cols), mActivationMatrix(mSize, -1),  mWeights(mSize * mSize, 0) {}

    // Constructor with memories
    HopfieldNetwork(int rows, int cols, const std::vector<std::vector<int>>& memories)
        : mRows(rows), mCols(cols), mActivationMatrix(rows * cols, -1), mWeights(mSize * mSize, 0), mMemories(memories) {}

    /**
     * Static constructor that reads a network architecture (rows, cols) and
     * memories from a file.
     * @note Memories are stored but network  are not changed YET!
     */
    static HopfieldNetwork fromFile(const std::string& filename);

    void saveToFile(const std::string& filename) const override;

    int rows() const override { return mRows; }
    int cols() const override { return mCols; }

    int& at(int row, int col) override { return mActivationMatrix[row * mCols + col]; }
    int at(int row, int col) const override { return mActivationMatrix[row * mCols + col]; }

    std::vector<int>& data() override { return mActivationMatrix; }
    const std::vector<int>& data() const override { return mActivationMatrix; }

    const std::vector<std::vector<int>>& getMemories() const override { return mMemories; }

    void addMemory(const std::vector<int>& memory) override { mMemories.push_back(memory); }

    void setActivations(const std::vector<int>& activations) override { mActivationMatrix = activations; }

private:
    int mRows, mCols, mSize;
    std::vector<int> mActivationMatrix; // -1 or 1
    std::vector<float> mWeights;
    std::vector<std::vector<int>> mMemories;
};

#endif // HOPFIELD_NETWORK