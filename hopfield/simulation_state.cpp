#include "hopfield/simulation_state.hpp"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


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
