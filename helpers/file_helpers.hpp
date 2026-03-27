#ifndef FILE_HELPERS
#define FILE_HELPERS

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

/**
 * Helper function for reading the entire contents of a text file into a string.
 * Returns an empty string if the file cannot be opened.
 */
static std::string readTextFile(const char* path) {
    std::ifstream file(path, std::ios::in);
    if (!file) {
        return {};
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

/**
 * Helper function to get a vector of lines from a text file
 */
static std::vector<std::string> getLinesFromFile(const char* path) {
    std::vector<std::string> lines;
    std::ifstream file(path, std::ios::in);
    if (!file) {
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

/*
 * Helper function to write to a file
 */
static void writeToFile(const char* text, const char* path) {
    std::ofstream file(path, std::ios::out);
    if (!file) {
        return;
    }
    file << text;
}

#endif // FILE_HELPERS