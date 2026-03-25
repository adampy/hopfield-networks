#ifndef FILE_HELPERS
#define FILE_HELPERS

#include <string>
#include <fstream>
#include <sstream>

/**
 * Helper function for reading the entire contents of a text file into a string.
 * Returns an empty string if the file cannot be opened.
 */
static std::string read_text_file(const char* path) {
    std::ifstream file(path, std::ios::in);
    if (!file)
        return {};

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

#endif // FILE_HELPERS