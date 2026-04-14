#include "hopfield/HopfieldNetwork.hpp"

#include <string>
#include <numeric> // for std::iota (to populate vectors)
#include <random> // for async updates (random node order)
#include <sstream> // for reading state.hpf header
#include <vector>
#include <iostream>
#include <Eigen/Dense>

#include "helpers/file_helpers.hpp"

using namespace Eigen;

HopfieldNetwork::HopfieldNetwork(int rows, int cols, StoragePrescription sp)
        : mRows(rows), mCols(cols), mSize(rows * cols),
        mActivationMatrix(mSize, -1),  mWeights(mSize * mSize, 0) {
    // empty constructor body

    mStoragePrescription = sp; // TODO: should we have initialiser list if we have constructor body?
}


HopfieldNetwork::HopfieldNetwork(int rows, int cols, StoragePrescription sp,
        const std::vector<std::vector<int>>& memories) :  HopfieldNetwork(rows, cols, sp) {
    
    mMemories = memories;
    mStoragePrescription = sp;

    setWeightsFromMemories();
}

HopfieldNetwork::HopfieldNetwork(const std::string& filename) : HopfieldNetwork(0, 0, HEBBIAN) {
    // Example format: (rows, cols, data)
    // 4 4 0
    // + - + -
    // - + - +
    // + - + -
    // - + - +
    // [blank lines allowed]
    // [more memories...]

    // Early return if state file empty
    const std::vector<std::string> lines = getLinesFromFile(filename.c_str());
    if (lines.empty()) {
        return;
    }

    // Read header parts
    int rows = 0;
    int cols = 0;
    int spInt = static_cast<int>(mStoragePrescription);
    const auto header = lines.at(0);
    std::istringstream hs(header);
    if (!(hs >> rows >> cols >> spInt)) {
        // Early return if header malformed
        return;
    }
    mRows = rows;
    mCols = cols;
    mSize = rows * cols;
    mStoragePrescription = static_cast<StoragePrescription>(spInt);

    // Read in memories
    std::vector<std::vector<int>> memories;
    std::vector<int> data;
    data.reserve(mSize);
    for (int i = 1; i < lines.size(); ++i) {
        const auto& line = lines.at(i);
        for (char c : line) {
            if (c == '+') {
                data.push_back(1);
            } else if (c == '-') {
                data.push_back(-1);
            }
        }

        // Add memory to vector
        if (data.size() == mSize) { // then we have read in a memory
            memories.push_back(data);
            data.clear();
            data.reserve(mSize);
        }
    }
    mMemories = std::move(memories);

    mActivationMatrix.assign(mSize, -1);
    mWeights.assign(mSize * mSize, 0.0f);
    if (!mMemories.empty()) {
        setWeightsFromMemories();
    }
}

void HopfieldNetwork::saveToFile(const std::string& filename) const {
    // Construct header (rows, columns, storage prescription)
    std::string toWrite = std::to_string(mRows) + " " + std::to_string(mCols)
        + " " + std::to_string(static_cast<int>(mStoragePrescription)) + "\n";
    
    // Construct list of memories
    for (const auto& memory : mMemories) {
        for (int value : memory) {
            if (value == 1) {
                toWrite += "+";
            } else {
                toWrite += "-";
            }
        }
        toWrite += "\n";
    }

    // Write contents to file
    writeToFile(toWrite.c_str(), filename.c_str());
}

MatrixXd HopfieldNetwork::createMemoryMatrix() {
    MatrixXd result(mSize, mMemories.size());

    for(int i = 0; i < mMemories.size();  i++){
        result.col(i) = VectorXi::Map(mMemories.at(i).data(), mSize).cast<double>();
    }

    return result;
}

// TODO: add functions to update weights rather than needing to calculate from scratch
void HopfieldNetwork::setWeightsFromMemories() {
    MatrixXd memoriesMatrix = createMemoryMatrix();
    MatrixXd weights(mSize, mSize);

    switch (mStoragePrescription) // TODO: add other storage prescriptions
    {
    case HEBBIAN:
        weights = (1.0 / mMemories.size()) *  memoriesMatrix * memoriesMatrix.transpose();
        break;
    
    default:
        std::cerr << "Error: Storage prescription has not been implemented";
        break;
    }

    mWeights = std::vector<float>(weights.data(), weights.data() + weights.rows() * weights.cols());
}

int HopfieldNetwork::getSign(int linearIndex) {
    float sum = 0;
    for(int otherNodeIndex = 0; otherNodeIndex < mSize; otherNodeIndex++){
        if (linearIndex != otherNodeIndex) {
            sum += mActivationMatrix[otherNodeIndex] * weightAt(linearIndex, otherNodeIndex);
        }
    }

    return sum > 0 ? 1 : -1; // TODO: Should this be >= instead of > ?
}

void HopfieldNetwork::updateNode(int linearIndex) {
    int sign = getSign(linearIndex);
    if( sign != mActivationMatrix[linearIndex]) {
        mActivationMatrix[linearIndex] = sign;
    }
}

// N.B. Not sure if the unit of interest is an individual node update or a full pass of node updates
void HopfieldNetwork::asyncUpdatePass(InferenceMode mode) {
    // Early return if sync inference selected
    if (mode == SYNC) {
        std::cerr << "Error: Sync update not supported in asyncUpdatePass" << std::endl;
        return;
    }
    // At this point, we have asynchronous inference which can either be
    // - ASYNC_RANDOM: randomly defined ordering
    // - ASYNC_SEQUENTIAL: sequentially defined ordering

    // Define update order - start with sequential and shuffle if req'd
    std::vector<int> ordering(mSize, -1);
    std::iota(ordering.begin(), ordering.end(), 0);
    if (mode == ASYNC_RANDOM) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(ordering.begin(), ordering.end(), g);
    }

    // Update nodes according to ordering
    for (int i = 0; i < mSize; i++) {
        updateNode(ordering[i]);
    }
} 

void HopfieldNetwork::infer(InferenceMode mode) {
    switch (mode) {
        case ASYNC_RANDOM:
        case ASYNC_SEQUENTIAL:
            asyncUpdatePass(mode);
            break;
        case SYNC:
            // TODO: implement sync update
            std::cerr << "Error: Sync update not implemented yet" << std::endl;
            break;
    }
}
