#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "experiment/ExperimentHelpers.hpp"
#include "helpers/FileHelpers.hpp"

std::string buildRunId(const std::vector<std::string>& parts) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            oss << "_";
        }
        oss << parts[i];
    }
    return oss.str();
}

std::filesystem::path prepareExperimentOutputDir(Experiment exp,
        const std::string& runId, std::error_code& fsErr) {
    const std::filesystem::path baseOutputDir = std::filesystem::path("data") / getExperimentName(exp);
    const std::filesystem::path outputDir = baseOutputDir / runId;
    std::filesystem::create_directories(outputDir, fsErr);
    return outputDir;
}

bool writeHeatmapCsv(const std::filesystem::path& outputDir, const bool useFallback,
        const std::string& fileStem, const std::string& header, const int xCount,
        const int yCount, const float yStep, const std::vector<std::vector<float>>& heatmap) {
    std::filesystem::path outputPath = outputDir / (fileStem + ".csv");
    if (useFallback) {
        outputPath = std::filesystem::path(fileStem + ".csv");
    }

    std::ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        return false;
    }

    outputFile << header << "\n";
    for (int x = 1; x < xCount + 1; ++x) {
        for (int y = 0; y < yCount; ++y) {
            const float yValue = y * yStep;
            outputFile << x << "," << yValue << "," << heatmap[x - 1][y] << "\n";
        }
    }
    outputFile.close();
    return true;
}

bool writeReadme(const std::filesystem::path& outputDir, const bool useFallback,
        const std::string& fileName, const std::vector<std::string>& lines) {
    std::filesystem::path readmePath = outputDir / fileName;
    if (useFallback) {
        readmePath = std::filesystem::path(fileName);
    }

    std::ostringstream oss;
    for (const auto& line : lines) {
        oss << line << "\n";
    }

    return writeTextFile(oss.str(), readmePath.string().c_str());
}

