#ifndef INFERENCE_MODE
#define INFERENCE_MODE

enum InferenceMode {
    ASYNC_RANDOM = 0,     // Random node update order
    ASYNC_SEQUENTIAL = 1, // Sequential node update order
    SYNC = 2              // Synchronous update (all nodes updated simultaneously)
};

#endif // INFERENCE_MODE