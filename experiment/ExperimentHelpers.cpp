#include "experiment/ExperimentHelpers.hpp"

#include <random>

std::vector<int> generateRandomPattern(const int size) {
    std::vector<int> pattern(size);
    std::random_device rd; // TODO: How to implement seeds?
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    for (int i = 0; i < size; i++) {
        pattern[i] = dis(gen) * 2 - 1;
    }
    return pattern;
}
