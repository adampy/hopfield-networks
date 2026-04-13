#include "hopfield/SimulationState.hpp"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <sstream>
#include <string>


std::vector<int> SimulationState::getParsedNetworkSizes() const {
    std::vector<int> sizes;
    // Parse from copy so underlying buffer isn't modified
    const std::string input(mNetworkSizes);
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        // Skip empty entries (trailing commas, consecutive commas, etc)
        if (token.empty()) {
            continue;
        }
        // atoi tolerates leading whitespace - invalid tokens become 0
        int value = atoi(token.c_str());
        if (value > 0) {
            sizes.push_back(value);
        }
    }
    return sizes;
}

void SimulationState::deleteNetworkAndWidget(GLFWwindow* window) {
    if (window != nullptr) {
        glfwSetWindowUserPointer(window, nullptr);
    }

    mShouldDrawNetwork = false;
    if (mWidget != nullptr) {
        mWidget->shutdown_gl();
        delete mWidget;
        mWidget = nullptr;
    }
    if (mNetwork != nullptr) {
        delete mNetwork;
        mNetwork = nullptr;
    }
}

void SimulationState::setNetwork(IHopfieldNetwork* network) {
    this->mNetwork = network;
    this->mRows = network->rows();
    this->mCols = network->cols();
}

int SimulationState::makeWidgetFromNetwork(GLFWwindow* window) {    
    this->mWidget = new HopfieldWidget(*mNetwork, 20, 20, 640, 640);
    if (!this->mWidget->init_gl()) {
        std::cerr << "Failed to create shader program\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    this->mWidget->setScale(mWidgetScale);
    mShouldDrawNetwork = true;
    glfwSetWindowUserPointer(window, this->mWidget);
    return 0;
}
