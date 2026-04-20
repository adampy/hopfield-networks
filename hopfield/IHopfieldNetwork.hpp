#ifndef IHOPFIELD_NETWORK
#define IHOPFIELD_NETWORK

#include <vector>
#include <string>
#include "hopfield/InferenceMode.hpp"

/**
 * Interface for a Hopfield network with activation matrix
 * and associated memories.
 * 
 * @author Adam Crawley
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

    virtual void setActivations(const std::vector<int>& activations) = 0;

    virtual void saveToFile(const std::string& filename) const = 0;

    virtual void infer(InferenceMode mode) = 0;
};

#endif // IHOPFIELD_NETWORK