#ifndef HOPFIELD_WIDGET
#define HOPFIELD_WIDGET

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "hopfield/hopfield_network.hpp"

class HopfieldWidget {
public:
    HopfieldWidget(HopfieldNetwork& data, int x, int y, int width, int height);

    bool init_gl();
    void shutdown_gl();
    void draw(int fbWidth, int fbHeight);
    void on_mouse_button(GLFWwindow* window, int button, int action, int mods);

private:
    struct RenderInstance {
        float cx, cy;
        float r, g, b;
    };

    void update_render_instances();

    static void set_instance_color_from_state(int s, RenderInstance& ri);

    HopfieldNetwork& mData;

    int mX, mY, mWidth, mHeight;
    float mScale = 0.18f;
    float mRadius = 0.18f * 0.5f;

    std::vector<RenderInstance> mRenderInstances = {};

    GLuint mProgram = 0;
    GLuint mVao = 0;
    GLuint mVbo = 0;
    GLuint mEbo = 0;
    GLuint mInstanceVbo = 0;
};

#endif // HOPFIELD_WIDGET