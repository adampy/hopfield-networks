#ifndef PATTERN_UTILS
#define PATTERN_UTILS

#include <vector>

extern std::vector<int> generateRandomPattern(const int patternSize);

extern std::vector<std::vector<int>> generateRandomPatterns(
    const int numPatterns, const int patternSize);

extern void infuseWithNoise(std::vector<int>& pattern,
    const float noiseLevel);

extern float computeOverlap(const std::vector<int>& pattern1,
    const std::vector<int>& pattern2);

#endif // PATTERN_UTILS
