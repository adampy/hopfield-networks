#ifndef IHOPFIELD_NETWORK
#define IHOPFIELD_NETWORK

#include <vector>
#include <string>

/**
 * Interface for a Hopfield network with weight matrix
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

    virtual std::vector<int>& data() = 0;
    virtual const std::vector<int>& data() const = 0;

    virtual const std::vector<std::vector<int>>& getMemories() const = 0;

    virtual void addMemory(const std::vector<int>& memory) = 0;

    virtual void setWeights(const std::vector<int>& weights) = 0;

    virtual void saveToFile(const std::string& filename) const = 0;
};

#endif // IHOPFIELD_NETWORK