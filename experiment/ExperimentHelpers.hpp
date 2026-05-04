#ifndef EXPERIMENT_HELPERS
#define EXPERIMENT_HELPERS

#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include "experiment/ExperimentRunner.hpp"
#include "helpers/PatternUtils.hpp"

/**
 * Build a run ID from a vector of strings (combined with "_")
 * @param parts The parts to build the run ID from
 * @return The run ID
 */
extern std::string buildRunId(const std::vector<std::string>& parts);

/**
 * Make output directory for experiment
 * 
 * @param[in] exp The experiment to make the output directory for
 * @param[in] runId The run ID to use for the output directory
 * @param[out] fsErr The error code to use for the output directory
 * @return The path to the output directory
 */
extern std::filesystem::path prepareExperimentOutputDir(
    Experiment exp, const std::string& runId, std::error_code& fsErr);

/**
 * Write an experiment README file
 */
extern bool writeReadme(const std::filesystem::path& outputDir,
    const bool useFallback, const std::string& fileName,
    const std::vector<std::string>& lines);

/**
 * Write a heatmap CSV file
 */
extern bool writeHeatmapCsv(const std::filesystem::path& outputDir,
    const bool useFallback, const std::string& fileStem, const std::string& header,
    const int xCount, const int yCount, const float yStep,
    const std::vector<std::vector<float>>& heatmap);

extern bool writeCapacityCsv(const std::filesystem::path& outputDir,
                     const bool useFallback,
                     const std::string& fileStem,
                     const std::string& header,
                     const std::vector<std::tuple<int, int, double>>& data);

extern bool writeCurveCsv(const std::filesystem::path& outputDir,
                   const bool useFallback,
                   const std::string& fileStem,
                   const std::string& header,
                   const std::vector<std::tuple<int, double, double>>& data);

#endif // EXPERIMENT_HELPERS