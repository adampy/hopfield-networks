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
    ExperimentRunner() :
        mExp1Running(false), mExp1Progress(0.0f),
        mExp2Running(false), mExp2Progress(0.0f),
        mExp3Running(false), mExp3Progress(0.0f),
        mExp4Running(false), mExp4Progress(0.0f) {}
    ~ExperimentRunner() { join(); } // explicit destructor to join threads

    // delete copy, move, assignment constructors/operators
    ExperimentRunner(const ExperimentRunner&) = delete;
    ExperimentRunner& operator=(const ExperimentRunner&) = delete;

    // getters
    bool isExperiment1Running() const { return mExp1Running.load(std::memory_order_relaxed); }
    bool isExperiment2Running() const { return mExp2Running.load(std::memory_order_relaxed); }
    bool isExperiment3Running() const { return mExp3Running.load(std::memory_order_relaxed); }
    bool isExperiment4Running() const { return mExp4Running.load(std::memory_order_relaxed); }
    float getExperiment1Progress() const { return mExp1Progress.load(std::memory_order_relaxed); }
    float getExperiment2Progress() const { return mExp2Progress.load(std::memory_order_relaxed); }
    float getExperiment3Progress() const { return mExp3Progress.load(std::memory_order_relaxed); }
    float getExperiment4Progress() const { return mExp4Progress.load(std::memory_order_relaxed); }

    // setters
    void setExperiment1Running(bool running) { mExp1Running.store(running, std::memory_order_relaxed); }
    void setExperiment2Running(bool running) { mExp2Running.store(running, std::memory_order_relaxed); }
    void setExperiment3Running(bool running) { mExp3Running.store(running, std::memory_order_relaxed); }
    void setExperiment4Running(bool running) { mExp4Running.store(running, std::memory_order_relaxed); }
    void setExperiment1Progress(float progress) { mExp1Progress.store(progress, std::memory_order_relaxed); }
    void setExperiment2Progress(float progress) { mExp2Progress.store(progress, std::memory_order_relaxed); }
    void setExperiment3Progress(float progress) { mExp3Progress.store(progress, std::memory_order_relaxed); }
    void setExperiment4Progress(float progress) { mExp4Progress.store(progress, std::memory_order_relaxed); }

    // Run experiments
    void startExperiment1(const std::vector<int>& sizes, const int numTrials);
    void startExperiment3(const int size, const int numTrials,
        const float maxSaturation, const float maxNoise,
        const float saturationRes = 0.01, const float noiseRes = 0.01);

private:
    std::atomic<bool> mExp1Running;
    std::atomic<bool> mExp2Running;
    std::atomic<bool> mExp3Running;
    std::atomic<bool> mExp4Running;

    std::atomic<float> mExp1Progress;
    std::atomic<float> mExp2Progress;
    std::atomic<float> mExp3Progress;
    std::atomic<float> mExp4Progress;
    
    std::thread mExp1Worker;
    std::thread mExp2Worker;
    std::thread mExp3Worker;
    std::thread mExp4Worker;

    void join();
};

#endif // EXPERIMENT_RUNNER_HPP
