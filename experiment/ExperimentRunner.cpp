#include "experiment/ExperimentRunner.hpp"
#include "experiment/ExperimentHelpers.hpp"
#include <chrono>
#include <iostream>


void ExperimentRunner::startExperiment1(std::vector<int> sizes, int numTrials) {
    if (isExperiment1Running()) {
        return;
    }

    join();

    setExperiment1Running(true);
    setExperiment1Progress(0.0f);

    mExp1Worker = std::thread([sizes = std::move(sizes), numTrials, this]() {
        const std::size_t total = sizes.size();
        if (total == 0) {
            setExperiment1Progress(1.0f);
            setExperiment1Running(false);
            return;
        }

        // EXPERIMENT GOES HERE

        const int patternSize = 100;

        for (std::size_t i = 0; i < total; ++i) {
            std::cout << "Running experiment for network size: " << sizes[i] << std::endl;
            setExperiment1Progress(static_cast<double>(i) / static_cast<double>(total));
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            const int networkSize = sizes[i];
            double bestCapacity = 0.0;

            for (int p = 1; p <= networkSize; ++p){
                int successCount = 0;

                for (int trial = 0; trial < numTrials; ++trial) {
                    std::vector<std::vector<int>> patterns = generateRandomPatterns(p, networkSize);

                    //auto weights = // train a hopfield network using patterns
                    
                    // pick a few patterns then remove them

                    // use the hopfield network to try to get them back

                    // check if it is successful or not


                }
            }

            // success rate

            // calculate a threshold

            setExperiment1Progress(static_cast<double>(i + 1) / static_cast<double>(total));
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
