#ifndef SIMULATION_STATE
#define SIMULATION_STATE

#include "hopfield/IHopfieldNetwork.hpp"
#include "ui/hopfield_widget.hpp"
 #include <cstddef>

/**
 * Manages the state of a Hopfield network simulation, including network itself,
 * display settings, simulation parameters.
 *
 * @author Adam Crawley
 */
class SimulationState {
public:
    enum StoragePrescription {
        HEBBIAN = 0,
        PSEUDO_INVERSE = 1,
        STORKEY = 2
    };

    SimulationState() : mCols(3), mRows(5), mShouldDrawNetwork(false), 
        mWidget(nullptr), mNetwork(nullptr),
        mStoragePrescription(StoragePrescription::HEBBIAN), mNoiseLevel(0.0f),
        mWidgetScale(1.0f), mNumTrials1(3), mNumTrials2(3), mRetrievalThreshold1(0.95),
        mRetrievalThreshold3(0.95), mNetworkSize(100), mMaxSaturation(0.1f) {}

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
    char* getNetworkSizes() { return mNetworkSizes; }
    std::size_t getNetworkSizesCapacity() const { return sizeof(mNetworkSizes); } // needed for ImGui::InputText
    int* getNumTrials1() { return &mNumTrials1; }
    int* getNumTrials2() { return &mNumTrials2; }
    float* getRetrievalThreshold1() { return &mRetrievalThreshold1; }
    float* getRetrievalThreshold3() { return &mRetrievalThreshold3; }
    int* getNetworkSize() { return &mNetworkSize; }
    float* getMaxSaturation() { return &mMaxSaturation; }
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

    // Experiment related data members
    char mNetworkSizes[256];
    int mNumTrials1; // for experiment 1
    int mNumTrials2; // for experiment 2
    float mRetrievalThreshold1; // for experiment 1
    float mRetrievalThreshold3; // for experiment 3
    int mNetworkSize;
    float mMaxSaturation;
    float mMaxNoise;
};

#endif // SIMULATION_STATE
