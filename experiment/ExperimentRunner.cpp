#include <chrono>
#include <iostream>
#include "experiment/ExperimentRunner.hpp"
#include "experiment/ExperimentHelpers.hpp"
#include "hopfield/HopfieldNetwork.hpp"
#include "hopfield/StoragePrescription.hpp"


bool ExperimentRunner::isRunning(Experiment exp) const {
    switch (exp) {
        case EXPERIMENT_1:
            return mExp1Running.load(std::memory_order_relaxed);
        case EXPERIMENT_2:
            return mExp2Running.load(std::memory_order_relaxed);
        case EXPERIMENT_3:
            return mExp3Running.load(std::memory_order_relaxed);
        default:
            return false;
    }
}

float ExperimentRunner::getProgress(Experiment exp) const {
    switch (exp) {
        case EXPERIMENT_1:
            return mExp1Progress.load(std::memory_order_relaxed);
        case EXPERIMENT_2:
            return mExp2Progress.load(std::memory_order_relaxed);
        case EXPERIMENT_3:
            return mExp3Progress.load(std::memory_order_relaxed);
        default:
            return 0.0f;
    }
}

void ExperimentRunner::setRunning(Experiment exp, bool running) {
    switch (exp) {
        case EXPERIMENT_1:
            mExp1Running.store(running, std::memory_order_relaxed);
            break;
        case EXPERIMENT_2:
            mExp2Running.store(running, std::memory_order_relaxed);
            break;
        case EXPERIMENT_3:
            mExp3Running.store(running, std::memory_order_relaxed);
            break;
        default:
            break;
    }
}

void ExperimentRunner::setProgress(Experiment exp, float progress) {
    switch (exp) {
        case EXPERIMENT_1:
            mExp1Progress.store(progress, std::memory_order_relaxed);
            break;
        case EXPERIMENT_2:
            mExp2Progress.store(progress, std::memory_order_relaxed);
            break;
        case EXPERIMENT_3:
            mExp3Progress.store(progress, std::memory_order_relaxed);
            break;
        default:
            break;
    }
}


void ExperimentRunner::startExperiment1(const std::vector<int>& sizes, const int numTrials) {
    if (isRunning(Experiment::EXPERIMENT_1)) {
        return;
    }

    join();

    setRunning(Experiment::EXPERIMENT_1, true);
    setProgress(Experiment::EXPERIMENT_1, 0.0f);

    mExp1Worker = std::thread([sizes, numTrials, this]() {
        const std::size_t numNetworkSizes = sizes.size();
        if (numNetworkSizes == 0) {
            setProgress(Experiment::EXPERIMENT_1, 1.0f);
            setRunning(Experiment::EXPERIMENT_1, false);
            return;
        }

        // EXPERIMENT GOES HERE
        const std::vector<StoragePrescription> storagePrescriptions = {HEBBIAN, PSEUDO_INVERSE, STORKEY};

        std::ostringstream thetaStr;
        thetaStr << std::fixed << std::setprecision(2) << 0.95;

        const std::string runId = buildRunId({
            "Exp1",
            "Sizes" + std::to_string(sizes.size()),
            "Trials" + std::to_string(numTrials),
            "Theta" + thetaStr.str()
        });

        std::error_code fsErr;
        const std::filesystem::path outputDir = prepareExperimentOutputDir(
            Experiment::EXPERIMENT_1, runId, fsErr);

        const bool useFallback = static_cast<bool>(fsErr);

        // These are used to track crude progress percentage
        const int numSubExperiments = numNetworkSizes * storagePrescriptions.size();
        int subExperimentIndex = 0;
        
        // for each storage prescription
        for (int spIndex = 0; spIndex < storagePrescriptions.size(); ++spIndex) { /// 
            const StoragePrescription storagePrescription = storagePrescriptions[spIndex];

            std::vector<std::tuple<int, int, double>> results; 
            // (networkSize, P_c, alpha_c)

            // for each network size
            for (std::size_t i = 0; i < numNetworkSizes; ++i) {
                const int networkSize = sizes[i];
                const int rows = networkSize / 2;
                const int cols = networkSize / 2;
                std::cout << "Running experiment for network size: " << networkSize << std::endl;
                setProgress(Experiment::EXPERIMENT_1,
                    static_cast<double>(subExperimentIndex) / static_cast<double>(numSubExperiments));

                double meanSuccessRate = 0.0;
                const int maxP = networkSize;
                int p = 0; // number of patterns to try store in network
                std::vector<double> successRates; // stores success rate for current p for each trial
                do {
                    p++;
                    successRates.clear();

                    for (int trial = 0; trial < numTrials; ++trial) {
                        const std::vector<std::vector<int>> patterns = generateRandomPatterns(p, networkSize);

                        // train a hopfield network using patterns
                        HopfieldNetwork network(rows, cols, storagePrescription, patterns);

                        int successCount = 0;

                        for (const auto& pattern : patterns) {
                            std::vector<int> noisyPattern = pattern;

                            // 10 percent noise
                            infuseWithNoise(noisyPattern, 0.1f);
                            network.setActivations(noisyPattern);

                            network.inferUntilConvergence(InferenceMode::ASYNC_RANDOM);

                            const std::vector<int> recalled = network.getActivations();

                            double overlap = computeOverlap(pattern, recalled);

                            // succeeded?
                            if (overlap >= 0.95) {
                                successCount++;
                            }
                        }

                        // need to work out the success rate
                        double successRate = static_cast<double>(successCount) / static_cast<double>(p); // remember p is the number of networks
                        successRates.push_back(successRate);
                    }

                    meanSuccessRate = 0.0;
                    for (double r : successRates) {
                        meanSuccessRate += r;
                    }

                    meanSuccessRate /= static_cast<double>(successRates.size());
                    // calculate meanSuccessRate from successRates

                } while (meanSuccessRate >= 0.95 && p < maxP); // TODO: Make 0.95 a function argument

                // max storeable patterns (P_c) is p-1
                int P_c = p -1;

                // critical storage capacity (alpha_c) is P_c / N
                double alpha_c = static_cast<double>(P_c) / static_cast<double>(networkSize);

                // store all alpha_c's then graph later
                results.emplace_back(networkSize, P_c, alpha_c);

                subExperimentIndex++; // used to track progress
            }

            writeCapacityCsv(
                outputDir,
                useFallback,
                storagePrescriptionToString(storagePrescription) + "_capacity",
                "networkSize,P_c,alpha_c",
                results
            );
        
        }
        

        const std::string readmeName = useFallback ? "experiment1_README.md" : "README.md";

        writeReadme(
            outputDir,
            useFallback,
            readmeName,
            {
                "### Inputs",
                "- Network sizes: vector<int> (count = " + std::to_string(sizes.size()) + ")",
                "- numTrials: " + std::to_string(numTrials),
                "- success threshold (theta): 0.95",
                "",
                "### Output",
                "- One CSV per storage rule",
                "- Columns: networkSize, P_c, alpha_c"
            });

        // END OF EXPERIMENT

        setProgress(Experiment::EXPERIMENT_1, 1.0f);
        setRunning(Experiment::EXPERIMENT_1, false);
    });
}

void ExperimentRunner::startExperiment2(const std::vector<int>& sizes, const int numTrials) {
    if (isRunning(Experiment::EXPERIMENT_2)) {
        return;
    }

    join();

    setRunning(Experiment::EXPERIMENT_2, true);
    setProgress(Experiment::EXPERIMENT_2, 0.0f);

    mExp2Worker = std::thread([sizes, numTrials, this]() {
        
        const std::vector<StoragePrescription> storagePrescriptions = {HEBBIAN, PSEUDO_INVERSE, STORKEY};

        std::ostringstream thetaStr;
        thetaStr << std::fixed << std::setprecision(2) << 0.95;

        const std::string runId = buildRunId({
            "Exp2",
            "Sizes" + std::to_string(sizes.size()),
            "Trials" + std::to_string(numTrials),
            "Theta" + thetaStr.str()
        });

        std::error_code fsErr;
        const std::filesystem::path outputDir = prepareExperimentOutputDir(
            Experiment::EXPERIMENT_2, runId, fsErr);

        const bool useFallback = static_cast<bool>(fsErr);

        const double noiseLevel = 0.1f;
        const double successThreshold = 0.95f;

        const std::size_t numNetworkSizes = sizes.size();
        const int numSubExperiments = numNetworkSizes * storagePrescriptions.size();
        int subExperimentIndex = 0;

        for (const auto& storagePrescription: storagePrescriptions) {

            for (std::size_t i = 0; i < numNetworkSizes; ++i) {

                const int N = sizes[i];

                std::vector<std::tuple<int, double, double>> curve;

                std::cout << "Running Experiment 2 network size: " << N << std::endl;

                setProgress(Experiment::EXPERIMENT_2, static_cast<double>(subExperimentIndex) / numSubExperiments);

                const int maxP = N;

                for (int p = 1; p <= maxP; ++p) {
                    std::vector<double> trialRates; // use it for working out success rate
                    const auto allPatterns = generateRandomPatterns(maxP, N);

                    for (int trial = 0; trial < numTrials; ++trial) {

                        std::vector<std::vector<int>> patterns(
                            allPatterns.begin(),
                            allPatterns.begin() + p
                        );

                        HopfieldNetwork network(1, N, storagePrescription, patterns);

                        int successCount = 0;

                        for (const auto& pattern : patterns) {
                            std::vector<int> noisy = pattern;

                            infuseWithNoise(noisy, noiseLevel);

                            network.setActivations(noisy);
                            network.inferUntilConvergence(InferenceMode::ASYNC_RANDOM);

                            double overlap = computeOverlap(pattern, network.getActivations());

                            if (overlap >= successThreshold) {
                                successCount++;
                            }

                        }

                        double successRate = static_cast<double>(successCount) / p;
                        trialRates.push_back(successRate);
                    }

                    double meanRate = 0.0;
                    for (double r : trialRates) meanRate += r;
                    meanRate /= trialRates.size();

                    double alpha = static_cast<double>(p) / static_cast<double>(N);

                    curve.emplace_back(p, alpha, meanRate);
                }

                writeCurveCsv(
                    outputDir,
                    useFallback,
                    storagePrescriptionToString(storagePrescription) + "_curve",
                    "p,alpha,P_success",
                    curve
                );

                subExperimentIndex++;
            }
        }

        const std::string readmeName = useFallback ? "experiment2_README.md" : "README.md";

        writeReadme(
            outputDir,
            useFallback,
            readmeName,
            {
                "### Inputs",
                "- Network sizes: vector<int> (count = " + std::to_string(sizes.size()) + ")",
                "- numTrials: " + std::to_string(numTrials),
                "- success threshold (theta): 0.95",
                "- noiseLevel: 0.10",
                "",
                "### Output",
                "- One CSV per storage rule",
                "- Columns: p, alpha, p_success"
            });

        setProgress(Experiment::EXPERIMENT_2, 1.0f);
        setRunning(Experiment::EXPERIMENT_2, false);
        
    } ); // thread ends here
} 

void ExperimentRunner::startExperiment3(const int size, const int numTrials,
        const int maxPatterns, const float maxNoise,
        const float noiseRes) {
    if (isRunning(Experiment::EXPERIMENT_3)) {
        return;
    }

    join();

    setRunning(Experiment::EXPERIMENT_3, true);
    setProgress(Experiment::EXPERIMENT_3, 0.0f);

    mExp3Worker = std::thread([size, numTrials, maxPatterns, maxNoise, noiseRes, this]() {
        const std::vector<StoragePrescription> storagePrescriptions = {HEBBIAN, PSEUDO_INVERSE, STORKEY};

        const float thetaSuccess = 0.95f;
        const InferenceMode inferenceMode = InferenceMode::ASYNC_SEQUENTIAL;
        const int maxInferenceIterations = 100;

        std::ostringstream maxNoiseStr;
        maxNoiseStr << std::fixed << std::setprecision(3) << maxNoise;

        std::ostringstream thetaStr;
        thetaStr << std::fixed << std::setprecision(2) << thetaSuccess;

        // Make run ID
        const std::string runId = buildRunId({
            "N" + std::to_string(size),
            "Trials" + std::to_string(numTrials),
            "MaxP" + std::to_string(maxPatterns),
            "MaxNoise" + maxNoiseStr.str(),
            "NoiseRes" + std::to_string(noiseRes),
            "Theta" + thetaStr.str(),
        });

        // Make output directory
        std::error_code fsErr;
        const std::filesystem::path outputDir = prepareExperimentOutputDir(
            Experiment::EXPERIMENT_3, runId, fsErr);
        const bool useFallback = static_cast<bool>(fsErr);

        // Calculate how many subexperiments there are
        const int numNoiseLevels = (maxNoise / noiseRes) + 1;
        const int attemptsPerTrialAndNoise = (maxPatterns * (maxPatterns + 1)) / 2;
        const int numSubExperiments = numTrials * numNoiseLevels * attemptsPerTrialAndNoise * storagePrescriptions.size();
        std::cout << "Total sub-experiments: " << numSubExperiments << std::endl;
        int subExperimentIndex = 0;

        for (int spIndex = 0; spIndex < storagePrescriptions.size(); ++spIndex) {
            const StoragePrescription storagePrescription = storagePrescriptions[spIndex];

            // Experiment
            setProgress(Experiment::EXPERIMENT_3,
                static_cast<double>(subExperimentIndex) / static_cast<double>(numSubExperiments));
            
            // initialise heatmap
            std::vector<std::vector<float>> heatmap(
                maxPatterns,
                std::vector<float>(numNoiseLevels, 0.0f)
            );

            // increase number of patterns to store
            for (int p = 1; p < maxPatterns + 1; ++p) {
                const float load = p / static_cast<float>(size);
                for (int t = 0; t < numTrials; ++t) {
                    const std::vector<std::vector<int>> patterns = generateRandomPatterns(p, size);
                    HopfieldNetwork network(1, size, storagePrescription, patterns); // learns patterns
                    
                    // try retrieve all patterns for different noise levels
                    for (int n = 0; n < numNoiseLevels; ++n) {
                        const float noise = n * noiseRes;
                        int successCount = 0;
                        for (int i = 0; i < p; ++i) { // NB: p == patterns.size()
                            
                            std::vector<int> noisyPattern = patterns[i];
                            infuseWithNoise(noisyPattern, noise);

                            network.setActivations(noisyPattern);

                            // run network until convergence or max iterations
                            network.inferUntilConvergence(inferenceMode, maxInferenceIterations);
                            
                            // check if retrieved pattern matches original pattern
                            const std::vector<int> retrievedPattern = network.getActivations();
                            if (computeOverlap(retrievedPattern, patterns[i]) >= thetaSuccess) {
                                successCount++;
                            }

                            subExperimentIndex++;
                            setProgress(Experiment::EXPERIMENT_3,
                                static_cast<double>(subExperimentIndex) / static_cast<double>(numSubExperiments));
                        }
                        // calculate success rate
                        const float successRate = static_cast<float>(successCount) / static_cast<float>(p);
                        heatmap[p - 1][n] += successRate;
                    }
                }
            }

            // normalise heatmap
            for (int p = 0; p < maxPatterns; ++p) {
                for (int n = 0; n < numNoiseLevels; ++n) {
                    heatmap[p][n] /= static_cast<float>(numTrials);
                }
            }

            writeHeatmapCsv(
                outputDir,
                useFallback,
                storagePrescriptionToString(storagePrescription),
                "numPatterns,noiseLevel,P_success",
                maxPatterns,
                numNoiseLevels,
                noiseRes,
                heatmap);
            
            
        }

        const std::string readmeName = useFallback ? "experiment3_README.md" : "README.md";
        writeReadme(
            outputDir,
            useFallback,
            readmeName,
            {
                "## Inputs",
                "- Network size (N): " + std::to_string(size),
                "- numTrials: " + std::to_string(numTrials),
                "- maxPatterns: " + std::to_string(maxPatterns),
                "- maxNoise: " + std::to_string(maxNoise),
                "- noiseRes: " + std::to_string(noiseRes),
                "- thetaSuccess (overlap threshold): " + std::to_string(thetaSuccess),
                "- inferenceMode: ASYNC_SEQUENTIAL",
                "- maxInferenceIterations: " + std::to_string(maxInferenceIterations),
            });

        // END OF EXPERIMENT
        setProgress(Experiment::EXPERIMENT_3, 1.0f);
        setRunning(Experiment::EXPERIMENT_3, false);
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
}
