#include <iostream>
#include "config.hpp"

#include "hopfield/hopfield_network.hpp"
#include "ui/hopfield_widget.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

static void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    HopfieldWidget* widget = static_cast<HopfieldWidget*>(glfwGetWindowUserPointer(window));
    if (widget != nullptr) {
        widget->on_mouse_button(window, button, action, mods);
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    // Request an OpenGL 3.3 Core context.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    // Make GLFW window
    GLFWwindow* window = glfwCreateWindow(Config::WIDTH, Config::HEIGHT, Config::TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL functions via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, Config::WIDTH, Config::HEIGHT); // Initial viewport
    glfwSwapInterval(1); // Optional: enable vsync (1 = on, 0 = off)
    glDisable(GL_DEPTH_TEST); // Disable depth
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Set widget mouse callback handler

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // State
    int cols = 3;
    int rows = 5;
    bool shouldDrawNetwork = false;
    HopfieldWidget* widget = nullptr;
    HopfieldNetwork* network = nullptr;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        // Render
        glClearColor(0.10f, 0.12f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        int fbW = 0, fbH = 0;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        
        if (widget != nullptr && network != nullptr && shouldDrawNetwork) {
            widget->draw(fbW, fbH);
        }

        // ----- ImGUI -----
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hopfield Network");
        ImGui::Text("This is a simple Hopfield network visualization.");

        if (ImGui::CollapsingHeader("Display")) {
            ImGui::Checkbox("Draw Network", &shouldDrawNetwork);
        }

        if (ImGui::CollapsingHeader("Network")) {
            // Load network button
            if (network == nullptr) {
                if (ImGui::Button("Load from state.hpf")) {
                    const std::string filePath = "state.hpf";
                    HopfieldNetwork loaded = HopfieldNetwork::fromFile(filePath);

                    if (widget != nullptr) {
                        widget->shutdown_gl();
                        delete widget;
                        widget = nullptr;
                    }
                    if (network != nullptr) {
                        delete network;
                        network = nullptr;
                    }

                    network = new HopfieldNetwork(std::move(loaded));
                    rows = network->rows();
                    cols = network->cols();

                    widget = new HopfieldWidget(*network, 20, 20, 640, 640);
                    if (!widget->init_gl()) {
                        std::cerr << "Failed to create shader program\n";
                        glfwDestroyWindow(window);
                        glfwTerminate();
                        return 1;
                    }
                    shouldDrawNetwork = true;
                    glfwSetWindowUserPointer(window, widget);
                }

                ImGui::SliderInt("Columns", &cols, 1, 10);
                ImGui::SliderInt("Rows", &rows, 1, 10);

                if (ImGui::Button("Create Network")) {
                    // Delete exisiting widget
                    if (widget != nullptr) {
                        widget->shutdown_gl();
                        delete widget;
                        widget = nullptr;
                    }
                    if (network != nullptr) {
                        delete network;
                        network = nullptr;
                    }

                    // Make new network
                    network = new HopfieldNetwork(rows, cols);
                    widget = new HopfieldWidget(*network, 20, 20, 640, 640);
                    if (!widget->init_gl()) {
                        std::cerr << "Failed to create shader program\n";
                        glfwDestroyWindow(window);
                        glfwTerminate();
                        return 1;
                    }
                    shouldDrawNetwork = true;
                    glfwSetWindowUserPointer(window, widget);
                }
            } else {
                if (ImGui::Button("Save to state.hpf")) {
                    network->saveToFile("state.hpf");
                }
                if (ImGui::Button("Clear Network")) {
                    if (widget != nullptr) {
                        widget->shutdown_gl();
                        delete widget;
                        widget = nullptr;
                    }
                    if (network != nullptr) {
                        delete network;
                        network = nullptr;
                    }
                }
            }
        }

        if (ImGui::CollapsingHeader("Memories")) {
            // If loaded, show memories and display button
            if (network != nullptr) {
                for (int i = 0; i < network->getMemories().size(); ++i) {
                    const auto& memory = network->getMemories().at(i);
                    if (ImGui::Button(("Set weights to memory " + std::to_string(i)).c_str())) {
                        network->setWeights(memory);
                        if (widget != nullptr) {
                            widget->setDirty();
                        }
                    }
                }

                if (ImGui::Button("Save Weights as New Memory")) {
                    const auto& weights = network->data();
                    network->addMemory(weights);
                }
            }
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Present frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (widget != nullptr) {
        widget->shutdown_gl();
        delete widget;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}