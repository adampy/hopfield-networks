#ifndef SIMULATION_STATE
#define SIMULATION_STATE

#include "hopfield/IHopfieldNetwork.hpp"
#include "ui/hopfield_widget.hpp"

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
        mWidgetScale(1.0f) {}

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
};

#endif // SIMULATION_STATE
