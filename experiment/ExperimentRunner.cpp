#include <chrono>
#include <iostream>
#include "experiment/ExperimentRunner.hpp"
#include "experiment/ExperimentHelpers.hpp"
#include "hopfield/HopfieldNetwork.hpp"
#include "hopfield/StoragePrescription.hpp"


void ExperimentRunner::startExperiment1(const std::vector<int>& sizes, const int numTrials) {
    if (isExperiment1Running()) {
        return;
    }

    join();

    setExperiment1Running(true);
    setExperiment1Progress(0.0f);

    mExp1Worker = std::thread([&sizes, numTrials, this]() {
        const std::size_t numNetworkSizes = sizes.size();
        if (numNetworkSizes == 0) {
            setExperiment1Progress(1.0f);
            setExperiment1Running(false);
            return;
        }

        // EXPERIMENT GOES HERE
        const std::vector<StoragePrescription> storagePrescriptions = {HEBBIAN, PSEUDO_INVERSE, STORKEY};
        
        // These are used to track crude progress percentage
        const int numSubExperiments = numNetworkSizes * storagePrescriptions.size();
        int subExperimentIndex = 0;
        
        // for each storage prescription
        for (int spIndex = 0; spIndex < storagePrescriptions.size(); ++spIndex) {
            const StoragePrescription storagePrescription = storagePrescriptions[spIndex];

            // for each network size
            for (std::size_t i = 0; i < numNetworkSizes; ++i) {
                const int networkSize = sizes[i];
                const int rows = networkSize / 2;
                const int cols = networkSize / 2;
                std::cout << "Running experiment for network size: " << networkSize << std::endl;
                setExperiment1Progress(static_cast<double>(subExperimentIndex) / static_cast<double>(numSubExperiments));

                // std::this_thread::sleep_for(std::chrono::milliseconds(500));

                
                double meanSuccessRate = 0.0;

                int p = 0; // number of patterns to try store in network
                std::vector<double> successRates; // stores success rate for current p for each trial
                do {
                    p++;
                    successRates.clear();

                    for (int trial = 0; trial < numTrials; ++trial) {
                        const std::vector<std::vector<int>> patterns = generateRandomPatterns(p, networkSize);

                        // train a hopfield network using patterns
                        HopfieldNetwork network(rows, cols, storagePrescription, patterns);
                        
                        // iterate throuh all patterns then remove them

                            // infuse pattern with noise (make noise a function argument with default 10% or 20%)

                            // use the hopfield network to try to get them back
                            //     (need to implement convergence detection - see report at end of 2.2.1 Asynchronous)
                            // 
                            //     code will look something like
                            //
                            //     std::vector<int> noisyPattern = pattern;
                            //     infuseWithNoise(noisyPattern, 0.1f);
                            //     network.setActivations(noisyPattern);
                            //     while not converged:
                            //         network.infer(InferenceMode::ASYNC_SEQUENTIAL); // OR ASYNC_RANDOM - it doesn't really matter
                            //     check overlap using network
                            //         computeOverlap(pattern, network.getActivations())
                            
                            // check if it is successful or not (i.e. overlap >= 0.95)

                                // if so add to successCount
                        
                        // add successCount/P to successRates

                    }
                    // calculate meanSuccessRate from successRates

                } while (meanSuccessRate >= 0.95); // TODO: Make 0.95 a function argument

                // max storeable patterns (P_c) is p-1

                // critical storage capacity (alpha_c) is P_c / N

                // store all alpha_c's then graph later

                subExperimentIndex++; // used to track progress
            }
        }

        // END OF EXPERIMENT

        setExperiment1Progress(1.0f);
        setExperiment1Running(false);
    });
}

void ExperimentRunner::startExperiment3(const int size, const int numTrials,
        const float maxSaturation, const float maxNoise,
        const float saturationRes, const float noiseRes) {
    if (isExperiment3Running()) {
        return;
    }

    join();

    setExperiment3Running(true);
    setExperiment3Progress(0.0f);

    mExp3Worker = std::thread([size, numTrials, maxSaturation, maxNoise, saturationRes, noiseRes, this]() {
        const std::vector<StoragePrescription> storagePrescriptions = {HEBBIAN, PSEUDO_INVERSE, STORKEY};

        // Calculate how many subexperiments there are
        std::cout << maxSaturation << " " << saturationRes << std::endl;
        std::cout << maxNoise << " " << noiseRes << std::endl;
        const int numSaturationLevels = (maxSaturation / saturationRes) + 1;
        const int numNoiseLevels = (maxNoise / noiseRes) + 1;
        const int numSubExperiments = numTrials * numSaturationLevels * numNoiseLevels * storagePrescriptions.size();
        std::cout << "Total sub-experiments: " << numSubExperiments << std::endl;
        int subExperimentIndex = 0;

        for (int spIndex = 0; spIndex < storagePrescriptions.size(); ++spIndex) {
            const StoragePrescription storagePrescription = storagePrescriptions[spIndex];

            // Experiment
            setExperiment3Progress(static_cast<double>(subExperimentIndex) / static_cast<double>(numSubExperiments));
            
            // initialise heatmap
            std::vector<std::vector<float>> heatmap(
                numSaturationLevels,
                std::vector<float>(numNoiseLevels, 0.0f)
            );

            for (int a = 0; a < numSaturationLevels; ++a) {
                double alpha = a * saturationRes; // alpha_a
                const int P = static_cast<int>(std::floor(alpha * size));
                
                std::cout << "alpha: " << alpha << " P: " << P << std::endl;
            }
            
        }

        // END OF EXPERIMENT

        setExperiment3Progress(1.0f);
        setExperiment3Running(false);
    });
}

void ExperimentRunner::join() {
    if (mExp1Worker.joinable()) {
        mExp1Worker.join();
    }
    
    if (mExp2Worker.joinable()) {
        mExp2Worker.join();
    }

    if (mExp3Worker.joinable()) {
        mExp3Worker.join();
    }

    if (mExp4Worker.joinable()) {
        mExp4Worker.join();
    }
}
