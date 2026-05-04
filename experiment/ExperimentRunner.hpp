#ifndef EXPERIMENT_RUNNER_HPP
#define EXPERIMENT_RUNNER_HPP

#include <atomic> // for atomic flags
#include <thread>
#include <vector>

/**
 * Enum for the different experiments
 */
enum Experiment {
    EXPERIMENT_1,
    EXPERIMENT_2,
    EXPERIMENT_3
};

inline std::string getExperimentName(Experiment exp) {
    switch (exp) {
        case EXPERIMENT_1:
            return "experiment1";
        case EXPERIMENT_2:
            return "experiment2";
        case EXPERIMENT_3:
            return "experiment3";
        default:
            return "unknown";
    }
}

/**
 * Utility class to run experiments in a worker thead so the
 * UI is not blocked
 * 
 * @author Adam Crawley
 */
class ExperimentRunner {
public:
    ExperimentRunner() :
        mExp1Running(false), mExp1Progress(0.0f),
        mExp2Running(false), mExp2Progress(0.0f),
        mExp3Running(false), mExp3Progress(0.0f) {}
    ~ExperimentRunner() { join(); } // explicit destructor to join threads

    // delete copy, move, assignment constructors/operators
    ExperimentRunner(const ExperimentRunner&) = delete;
    ExperimentRunner& operator=(const ExperimentRunner&) = delete;

    // getters
    bool isRunning(Experiment exp) const;
    float getProgress(Experiment exp) const;
    
    // setters
    void setRunning(Experiment exp, bool running);
    void setProgress(Experiment exp, float progress);

    // Run experiments
    void startExperiment1(const std::vector<int>& size, const int numTrials); // TODO: Maybe make a "startExperiment" function?
    void startExperiment2(const std::vector<int>& sizes, const int numTrials);
    void startExperiment3(const int size, const int numTrials,
        const int maxPatterns, const float maxNoise,
        const float noiseRes = 0.01);

private:
    std::atomic<bool> mExp1Running;
    std::atomic<bool> mExp2Running;
    std::atomic<bool> mExp3Running;

    std::atomic<float> mExp1Progress;
    std::atomic<float> mExp2Progress;
    std::atomic<float> mExp3Progress;
    
    std::thread mExp1Worker;
    std::thread mExp2Worker;
    std::thread mExp3Worker;

    void join();
};

#endif // EXPERIMENT_RUNNER_HPP
