#ifndef IHOPFIELD_NETWORK
#define IHOPFIELD_NETWORK

#include <vector>
#include <string>
#include "hopfield/InferenceMode.hpp"
#include "hopfield/StoragePrescription.hpp"

/**
 * Interface for a Hopfield network with activation matrix
 * and associated memories.
 * 
 * @author Adam Crawley
 * @author William Kingdon
 */
class IHopfieldNetwork {
public:
    virtual ~IHopfieldNetwork() = default;

    virtual int rows() const = 0;
    virtual int cols() const = 0;

    virtual int& at(int row, int col) = 0;
    virtual int at(int row, int col) const = 0;

    virtual double weightAt(int row1, int col1, int row2, int col2) const = 0;

    virtual std::vector<int>& data() = 0;
    virtual const std::vector<int>& data() const = 0;

    virtual const std::vector<std::vector<int>>& getMemories() const = 0;

    virtual void addMemory(const std::vector<int>& memory, bool recompute = false) = 0;

    virtual void removeMemory(const int index, bool recompute = false) = 0;

    virtual const std::vector<int>& getActivations() const = 0;

    virtual void setActivations(const std::vector<int>& activations) = 0;

    virtual void saveToFile(const std::string& filename) const = 0;

    /**
     * Perform a single entire network pass of inference
     * @note No convergence checking included (since this is a single pass)
     */
    virtual void infer(InferenceMode mode) = 0;
    
    /**
     * Perform inference until convergence or max iterations reached
     */
    virtual void inferUntilConvergence(InferenceMode mode,const int maxIterations = 1000) = 0;

    virtual StoragePrescription getStoragePrescription() const = 0;

    virtual void setStoragePrescription(StoragePrescription sp) = 0;

    /**
     * Recalculate network weights using the stored memories and storage
     * prescription.
     */
    virtual void setWeightsFromMemories() = 0;

    /**
     * Calculate the energy of the current state of the network
     * @return The energy of the network
     */
    virtual double getEnergy() const = 0;
};

#endif // IHOPFIELD_NETWORK