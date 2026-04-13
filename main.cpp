#include <iostream>
#include "config.hpp"

#include "hopfield/ExperimentRunner.hpp"
#include "hopfield/SimulationState.hpp"
#include "hopfield/HopfieldNetwork.hpp"
#include "hopfield/StoragePrescription.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ui/hopfield_widget.hpp"

static void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    HopfieldWidget* widget = static_cast<HopfieldWidget*>(glfwGetWindowUserPointer(window));
    if (widget != nullptr) {
        widget->on_mouse_button(window, button, action, mods);
    }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    HopfieldWidget* widget = static_cast<HopfieldWidget*>(glfwGetWindowUserPointer(window));
    if (widget != nullptr) {
        widget->on_cursor_pos(window, xpos, ypos);
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
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Main loop
    SimulationState state;
    ExperimentRunner experiments;

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        // Render
        glClearColor(0.10f, 0.12f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        int fbW = 0, fbH = 0;
        glfwGetFramebufferSize(window, &fbW, &fbH);

        // UI Logic
        if (state.getWidget() != nullptr && state.getNetwork() != nullptr
                && *state.getShouldDrawNetwork()) {
            state.getWidget()->setScale(*state.getWidgetScale());
            state.getWidget()->draw(fbW, fbH);
        }

        // ----- ImGUI -----
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Make window for experiment progress if an experiment is running
        if (experiments.isExperiment1Running()) {
            ImGui::Begin("Experiment Progress", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Running Experiment 1...");
            const float p = experiments.getExperiment1Progress();
            ImGui::ProgressBar(p, ImVec2(300, 0));
            ImGui::End();
        }

        ImGui::Begin("Hopfield Network");
        ImGui::Text("This is a simple Hopfield network visualization.");

        if (ImGui::CollapsingHeader("Display")) {
            ImGui::Checkbox("Draw Network", state.getShouldDrawNetwork());
            ImGui::SliderFloat("Scale", state.getWidgetScale(), 0.25f, 2.0f);
        }

        if (ImGui::CollapsingHeader("Network")) {
            // Load network button
            if (state.getNetwork() == nullptr) {
                if (ImGui::Button("Load from state.hpf")) {
                    state.deleteNetworkAndWidget(window);
                    state.setNetwork(new HopfieldNetwork(HopfieldNetwork::fromFile("state.hpf")));
                    if (state.makeWidgetFromNetwork(window) != 0) {
                        return 1;
                    }
                }

                ImGui::SliderInt("Columns", state.getCols(), 1, 10);
                ImGui::SliderInt("Rows", state.getRows(), 1, 10);

                if (ImGui::Button("Create Network")) {
                    // Delete exisiting widget
                    state.deleteNetworkAndWidget(window);

                    // Make new network
                    state.setNetwork(new HopfieldNetwork(*state.getRows(), *state.getCols()));
                    if (state.makeWidgetFromNetwork(window) != 0) {
                        return 1;
                    }
                }
            } else {
                if (ImGui::Button("Save to state.hpf")) {
                    state.getNetwork()->saveToFile("state.hpf");
                }
                if (ImGui::Button("Clear Network")) {
                    state.deleteNetworkAndWidget(window);
                }
            }
        }

        if (ImGui::CollapsingHeader("Memories")) {
            // If loaded, show memories and display button
            if (state.getNetwork() != nullptr) {
                for (int i = 0; i < state.getNetwork()->getMemories().size(); ++i) {
                    const auto& memory = state.getNetwork()->getMemories().at(i);
                    if (ImGui::Button(("Set Activations to memory " + std::to_string(i)).c_str())) {
                        state.getNetwork()->setActivations(memory);
                        if (state.getWidget() != nullptr) {
                            state.getWidget()->setDirty();
                        }
                    }
                }

                if (ImGui::Button("Save Activations as New Memory")) {
                    state.getNetwork()->addMemory(state.getNetwork()->data());
                }
            }
        }

        if (ImGui::CollapsingHeader("Learning")) {
            if (state.getNetwork() == nullptr) {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "No network loaded");
            } else {
                const int memories = static_cast<int>(state.getNetwork()->getMemories().size());
                if (memories == 0) {
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "No memories loaded");
                } else {
                    ImGui::Text("Loaded %d memories", memories);

                    // radio buttons on storage prescription
                    ImGui::Text("Storage prescription:");
                    ImGui::RadioButton("Hebbian", state.getStoragePrescription(),
                        StoragePrescription::HEBBIAN);
                    ImGui::RadioButton("Pseudo-inverse", state.getStoragePrescription(),
                        StoragePrescription::PSEUDO_INVERSE);
                    ImGui::RadioButton("Storkey", state.getStoragePrescription(),
                        StoragePrescription::STORKEY);

                    if (ImGui::Button("Learn memories")) {
                        if (state.getNetwork() != nullptr) {
                            // use learning method with state.getNetwork()->getMemories() to learn weights
                        }
                    }
                }
            }
        }

        // Add separator with some padding
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (ImGui::CollapsingHeader("Experiment 1")) {
            ImGui::Text("Estimating critical capacity vs network size");
            ImGui::InputText("Network Sizes (comma separated)", state.getNetworkSizes(), state.getNetworkSizesCapacity());
            ImGui::InputInt("Number of trials##exp1", state.getNumTrials1());
            ImGui::SliderFloat("Retrieval Success Threshold##exp1", state.getRetrievalThreshold1(), 0.0f, 1.0f);
            const bool exp1IsRunning = experiments.isExperiment1Running();
            if (exp1IsRunning) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Run Experiment 1")) {
                const auto sizes = state.getParsedNetworkSizes();
                experiments.startExperiment1(sizes);
            }
            if (exp1IsRunning) {
                ImGui::EndDisabled();
            }
        }

        if (ImGui::CollapsingHeader("Experiment 2")) {
            ImGui::Text("3x Heatmaps of retrieval success vs saturation and noise level");
            ImGui::InputInt("Network Size", state.getNetworkSize());
            ImGui::InputInt("Number of trials##exp2", state.getNumTrials2());
            ImGui::SliderFloat("Max Saturation", state.getMaxSaturation(), 0.0f, 1.0f);
            ImGui::SliderFloat("Max Noise", state.getMaxNoise(), 0.0f, 1.0f);
            if (ImGui::Button("Run Experiment 2")) {
                // TODO: Run experiment
            }
        }

        if (ImGui::CollapsingHeader("Experiment 3")) {
            ImGui::Text("Critical Noise (i.e. noise level where all patterns retrieved with given threshold) vs saturation");
            ImGui::SliderFloat("Retrieval Success Threshold##exp3", state.getRetrievalThreshold3(), 0.0f, 1.0f);
            if (ImGui::Button("Run Experiment 3")) {
                // TODO: Run experiment
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
    state.deleteNetworkAndWidget(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}