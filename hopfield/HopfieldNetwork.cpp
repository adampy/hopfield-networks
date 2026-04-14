#include "hopfield/HopfieldNetwork.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <Eigen/Dense>

#include "helpers/file_helpers.hpp"

using namespace Eigen;

HopfieldNetwork::HopfieldNetwork(int rows, int cols, StoragePrescription sp)
    : mRows(rows), mCols(cols), mSize(rows * cols), mActivationMatrix(mSize, -1),  mWeights(mSize * mSize, 0) {}


HopfieldNetwork::HopfieldNetwork(int rows, int cols, StoragePrescription sp, const std::vector<std::vector<int>>& memories)
    :  HopfieldNetwork(rows, cols, sp) {
    
    mMemories = memories;

    setWeightsFromMemories();
}

// TODO: assess if this should be changed ito a constructor, and if it should include a storage prescription
HopfieldNetwork HopfieldNetwork::fromFile(const std::string& filename) {
    // Example format: (rows, cols, data)
    // 4 4
    // + - + -
    // - + - +
    // + - + -
    // - + - +
    // [blank lines allowed]
    // [more memories...]

    // TODO: Read this from file later...
    StoragePrescription sp = HEBBIAN;

    // Get network data from header
    const std::vector<std::string> lines = getLinesFromFile(filename.c_str());
    if (lines.empty()) {
        return HopfieldNetwork(0, 0, sp);
    }
    const auto header = lines.at(0);
    const auto sep = header.find(' ');
    if (sep == std::string::npos) {
        return HopfieldNetwork(0, 0, sp);
    }
    const int rows = std::stoi(header.substr(0, sep));
    const int cols = std::stoi(header.substr(sep + 1));

    // Read in memories
    std::vector<std::vector<int>> memories;
    std::vector<int> data;
    data.reserve(static_cast<size_t>(rows * cols));
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
        if (data.size() == rows * cols) { // then we have read in a memory
            memories.push_back(data);
            data.clear();
            data.reserve(static_cast<size_t>(rows * cols));
        }
    }

    HopfieldNetwork net(rows, cols, sp, memories);
    return net;
}

void HopfieldNetwork::saveToFile(const std::string& filename) const {
    // Write header
    std::string toWrite = std::to_string(mRows) + " " + std::to_string(mCols) + "\n";
    
    // Write memories
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

    writeToFile(toWrite.c_str(), filename.c_str());
}

MatrixXd HopfieldNetwork::createMemoryMatrix() {
    MatrixXd result(mMemories.size(), mSize);

    for(int i = 0; i < mMemories.size();  i++){
        result.row(i) = VectorXi::Map(mMemories.at(i).data(), mSize).cast<double>();
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

int HopfieldNetwork::getSign(int linearIndex){
    float sum = 0;
    for(int otherNodeIndex = 0; otherNodeIndex < mSize; otherNodeIndex++){
        if (linearIndex != otherNodeIndex) {
            sum += mActivationMatrix[otherNodeIndex] * weightAt(linearIndex, otherNodeIndex);
        }
    }

    return sum > 0 ? 1 : -1;
}

void HopfieldNetwork::updateNode(int linearIndex){
    int sign = getSign(linearIndex);
    if( sign != mActivationMatrix[linearIndex]) { // TODO: may want some kind of way to have a hook to the frontend?
        mActivationMatrix[linearIndex] = sign;
    }
}

// N.B. Not sure if the unit of interest is an individual node update or a full pass of node updates
void HopfieldNetwork::asyncUpdatePass() {
    for (int node = 0; node < mSize; node ++) {
        updateNode(node);
    }
} 