#ifndef EXPERIMENT_RUNNER_HPP
#define EXPERIMENT_RUNNER_HPP

#include <atomic> // for atomic flags
#include <thread>
#include <vector>

/**
 * Utility class to run experiments in a worker thead so the
 * UI is not blocked
 * 
 * @author Adam Crawley
 */
class ExperimentRunner {
public:
    ExperimentRunner() : mExp1Running(false), mExp1Progress(0.0f) {}
    ~ExperimentRunner() { join(); } // explicit destructor to join threads

    // delete copy, move, assignment constructors/operators
    ExperimentRunner(const ExperimentRunner&) = delete;
    ExperimentRunner& operator=(const ExperimentRunner&) = delete;

    // getters
    bool isExperiment1Running() const { return mExp1Running.load(std::memory_order_relaxed); }
    float getExperiment1Progress() const { return mExp1Progress.load(std::memory_order_relaxed); }

    // setters
    void setExperiment1Running(bool running) { mExp1Running.store(running, std::memory_order_relaxed); }
    void setExperiment1Progress(float progress) { mExp1Progress.store(progress, std::memory_order_relaxed); }

    // Run experiments
    void startExperiment1(std::vector<int> sizes, int numTrials);

private:
    std::atomic<bool> mExp1Running;
    std::atomic<float> mExp1Progress;
    std::thread mExp1Worker;

    void join();
};

#endif // EXPERIMENT_RUNNER_HPP
