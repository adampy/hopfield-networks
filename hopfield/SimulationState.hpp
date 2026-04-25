#ifndef SIMULATION_STATE
#define SIMULATION_STATE

#include "hopfield/IHopfieldNetwork.hpp"
#include "hopfield/StoragePrescription.hpp"
#include "ui/HopfieldWidget.hpp"
 #include <cstddef>

/**
 * Manages the state of a Hopfield network simulation, including network itself,
 * display settings, simulation parameters.
 *
 * @author Adam Crawley
 */
class SimulationState {
public:

    SimulationState() :
        mCols(3), mRows(5), mShouldDrawNetwork(false),
        mWidget(nullptr), mNetwork(nullptr),
        mStoragePrescription(StoragePrescription::HEBBIAN), mNoiseLevel(0.0f),
        mWidgetScale(1.0f), mInfusionNoiseLevel(0.f), mNumTrials1(3),
        mNumTrials2(3), mNumTrials3(3), mRetrievalThreshold1(0.95),
        mNetworkSize2(100), mNetworkSize3(100), mMaxPatterns(10),
        mMaxNoise(0.1f) {}

    std::vector<int> getParsedNetworkSizes() const;

    void deleteNetworkAndWidget(GLFWwindow* window);

    void setNetwork(IHopfieldNetwork* network);
    int makeWidgetFromNetwork(GLFWwindow* window);

    IHopfieldNetwork* getNetwork() { return mNetwork; }
    HopfieldWidget* getWidget() { return mWidget; }

    int* getCols() { return &mCols; }
    int* getRows() { return &mRows; }
    bool* getShouldDrawNetwork() { return &mShouldDrawNetwork; }
    int* getStoragePrescription() { return &mStoragePrescription; }
    float* getNoiseLevel() { return &mNoiseLevel; }
    float* getWidgetScale() { return &mWidgetScale; }
    float* getInfusionNoiseLevel() { return &mInfusionNoiseLevel; }

    char* getNetworkSizes() { return mNetworkSizes; }
    std::size_t getNetworkSizesCapacity() const { return sizeof(mNetworkSizes); } // needed for ImGui::InputText
    int* getNumTrials1() { return &mNumTrials1; }
    int* getNumTrials2() { return &mNumTrials2; }
    int* getNumTrials3() { return &mNumTrials3; }
    float* getRetrievalThreshold1() { return &mRetrievalThreshold1; }
    int* getNetworkSize2() { return &mNetworkSize2; }
    int* getNetworkSize3() { return &mNetworkSize3; }
    int* getMaxPatterns() { return &mMaxPatterns; }
    float* getMaxNoise() { return &mMaxNoise; }

private:
    // Hopfield related data members
    HopfieldWidget* mWidget;
    IHopfieldNetwork* mNetwork;

    // Display related data members
    int mCols;
    int mRows;
    bool mShouldDrawNetwork;
    int mStoragePrescription;
    float mNoiseLevel;
    float mWidgetScale;
    float mInfusionNoiseLevel;

    // Experiment related data members
    char mNetworkSizes[256];
    int mNumTrials1; // for experiment 1
    int mNumTrials2; // for experiment 2
    int mNumTrials3; // for experiment 3
    float mRetrievalThreshold1; // for experiment 1
    int mNetworkSize2;
    int mNetworkSize3;
    int mMaxPatterns;
    float mMaxNoise;
};

#endif // SIMULATION_STATE
