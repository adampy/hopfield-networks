#ifndef STORAGE_PRESCRIPTION
#define STORAGE_PRESCRIPTION

#include <string>

enum StoragePrescription {
    HEBBIAN = 0,
    PSEUDO_INVERSE = 1,
    STORKEY = 2
};

inline std::string storagePrescriptionToString(StoragePrescription prescription) {
    switch (prescription) {
        case HEBBIAN:
            return "Hebbian";
        case PSEUDO_INVERSE:
            return "PseudoInverse";
        case STORKEY:
            return "Storkey";
        default:
            return "unknown";
    }
}

#endif // STORAGE_PRESCRIPTION