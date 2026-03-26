#include <iostream>
#include "config.hpp"
#include "ui/hopfield_widget.hpp"

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

    // Initial viewport
    glViewport(0, 0, Config::WIDTH, Config::HEIGHT);

    // Optional: enable vsync (1 = on, 0 = off)
    glfwSwapInterval(1);

    HopfieldWidget widget(3, 5, 20, 20, 640, 640);
    if (!widget.init_gl()) {
        std::cerr << "Failed to create shader program\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // Disable depth
    glDisable(GL_DEPTH_TEST);

    // Set widget mouse callback handler
    glfwSetWindowUserPointer(window, &widget);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        // Render
        glClearColor(0.10f, 0.12f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        int fbW = 0, fbH = 0;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        widget.draw(fbW, fbH);

        // Present frame buffer
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Shutdown
    widget.shutdown_gl();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}