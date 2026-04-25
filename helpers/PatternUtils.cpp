#include "helpers/PatternUtils.hpp"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

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
    // Clamp noise between 0 and 1 inclusive
    const float p = std::max(0.f, std::min(1.f, noiseLevel));
    
    // Infuse noise
    std::random_device rd; // generates random seed
    std::mt19937 rng(rd());
    std::bernoulli_distribution flipDist(p);
    for (int i = 0; i < pattern.size(); i++) {
        if (flipDist(rng)) {
            pattern[i] = -pattern[i];
        }
    }
}

float computeOverlap(const std::vector<int>& pattern1,
        const std::vector<int>& pattern2) {
    if (pattern1.size() != pattern2.size()) {
        throw std::invalid_argument("Patterns must be the same size");
    }
    
    int overlapSum = 0;
    for (int i = 0; i < pattern1.size(); i++) {
        overlapSum += pattern1[i] * pattern2[i];
    }
    
    return static_cast<float>(overlapSum) / static_cast<float>(pattern1.size());
}
