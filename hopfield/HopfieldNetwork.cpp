#include "hopfield/HopfieldNetwork.hpp"

#include <string>
#include <vector>

#include "helpers/file_helpers.hpp"

HopfieldNetwork HopfieldNetwork::fromFile(const std::string& filename) {
    // Example format: (rows, cols, data)
    // 4 4
    // + - + -
    // - + - +
    // + - + -
    // - + - +
    // [blank lines allowed]
    // [more memories...]

    // Get network data from header
    const std::vector<std::string> lines = getLinesFromFile(filename.c_str());
    if (lines.empty()) {
        return HopfieldNetwork(0, 0);
    }
    const auto header = lines.at(0);
    const auto sep = header.find(' ');
    if (sep == std::string::npos) {
        return HopfieldNetwork(0, 0);
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

    HopfieldNetwork net(rows, cols, memories);
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