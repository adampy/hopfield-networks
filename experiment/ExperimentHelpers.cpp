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

std::vector<std::vector<int>> generateRandomPatterns(
        const int numPatterns, const int patternSize) {
    std::vector<std::vector<int>> patterns(numPatterns);
    for (int i = 0; i < numPatterns; i++) {
        patterns[i] = generateRandomPattern(patternSize);
    }
    return patterns;
}

void infuseWithNoise(std::vector<int>& pattern,
        const float noiseLevel) {
    // implementation goes here
}

float computeOverlap(const std::vector<int>& pattern1,
        const std::vector<int>& pattern2) {
    // implementation goes here
    return 0.f;
}

