#ifndef HOPFIELD_NETWORK
#define HOPFIELD_NETWORK

#include <vector>
#include <string>
#include "hopfield/StoragePrescription.hpp"
#include "hopfield/IHopfieldNetwork.hpp"

/**
 * Representation of a classical Hopfield network with activation matrix
 * and associated memories.
 
 * @author Adam Crawley
 */
class HopfieldNetwork : public IHopfieldNetwork {
public:
    // Constructor with no data
    HopfieldNetwork(int rows, int cols, StoragePrescription sp);

    // Constructor with memories
    HopfieldNetwork(int rows, int cols, StoragePrescription sp, const std::vector<std::vector<int>>& memories);
        // : mRows(rows), mCols(cols), mActivationMatrix(rows * cols, -1), mWeights(mSize * mSize, 0), mMemories(memories) {}

    /**
     * Static constructor that reads a network architecture (rows, cols) and
     * memories from a file.
     * @note Memories are stored but network  are not changed YET!
     */
    static HopfieldNetwork fromFile(const std::string& filename);

    // Get current state:
    void saveToFile(const std::string& filename) const override;

    int rows() const override { return mRows; }
    int cols() const override { return mCols; }

    int getLinearIndex(int row, int col) const  { return row * mCols + col; }
    int getLinearIndex(int row1, int col1, int row2, int col2) const { return mSize * getLinearIndex(row1, col1) + getLinearIndex(row2, col2); }

    int& at(int row, int col) override { return mActivationMatrix[getLinearIndex(row, col)]; } // This can edit values so should be private?
    int at(int row, int col) const override { return mActivationMatrix[getLinearIndex(row, col)]; }

    float weightAt(int row1, int col1, int row2, int col2) const override { return mWeights[getLinearIndex(row1, col1, row2, col2)]; }
    float weightAt(int linear1, int linear2) const { return mWeights[linear1 * mSize + linear2]; }


    std::vector<int>& data() override { return mActivationMatrix; }
    const std::vector<int>& data() const override { return mActivationMatrix; }
    
    const std::vector<std::vector<int>>& getMemories() const override { return mMemories; }

    // Change current state
    void addMemory(const std::vector<int>& memory) override { mMemories.push_back(memory); } // this should add memory and add associated weights
    
    void setActivations(const std::vector<int>& activations) override { mActivationMatrix = activations; }
    
    private:
    // N.B. should probably add some of this to the interface, will do that later
    int mRows, mCols, mSize;
    std::vector<int> mActivationMatrix; // -1 or 1
    std::vector<float> mWeights;
    std::vector<std::vector<int>> mMemories;
    StoragePrescription mStoragePrescription;
    
    float& weightAt(int row1, int col1, int row2, int col2) { return mWeights[getLinearIndex(row1, col1, row2, col2)]; }
    float& weightAt(int linear1, int linear2) { return mWeights[linear1 * mSize + linear2]; }

    void updateWeight(int row1, int col1, int row2, int col2, float weight) { 
        weightAt(row1, col1, row2, col2) = weightAt(row2, col2, row1, col1) = weight;
    }
    void updateWeight(int linear1, int linear2, float weight) { 
        weightAt(linear1, linear2) = weightAt(linear2, linear1) = weight;
    }

    void setWeightsFromMemories();
};

#endif // HOPFIELD_NETWORK