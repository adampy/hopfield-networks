#include "hopfield/ExperimentRunner.hpp"
#include <chrono>
#include <iostream>


void ExperimentRunner::startExperiment1(std::vector<int> sizes) {
    if (isExperiment1Running()) {
        return;
    }

    join();

    setExperiment1Running(true);
    setExperiment1Progress(0.0f);

    mExp1Worker = std::thread([sizes = std::move(sizes), this]() {
        const std::size_t total = sizes.size();
        if (total == 0) {
            setExperiment1Progress(1.0f);
            setExperiment1Running(false);
            return;
        }

        // EXPERIMENT GOES HERE

        for (std::size_t i = 0; i < total; ++i) {
            std::cout << "Running experiment for network size: " << sizes[i] << std::endl;
            setExperiment1Progress(static_cast<float>(i) / static_cast<float>(total));
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // END OF EXPERIMENT

        setExperiment1Progress(1.0f);
        setExperiment1Running(false);
    });
}

void ExperimentRunner::join() {
    if (mExp1Worker.joinable()) {
        mExp1Worker.join();
    }
}
