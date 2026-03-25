#ifndef HOPFIELD_WIDGET
#define HOPFIELD_WIDGET

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class HopfieldWidget {
public:
    HopfieldWidget(int x, int y, int width, int height);

    bool init_gl();
    void shutdown_gl();

    void draw(int fbWidth, int fbHeight);
    void on_mouse_button(GLFWwindow* window, int button, int action, int mods);

private:
    struct RenderInstance {
        float cx;
        float cy;
        float r;
        float g;
        float b;
    };

    static void set_instance_color_from_state(int s, RenderInstance& ri);

    int mX = 0;
    int mY = 0;
    int mWidth = 1;
    int mHeight = 1;
    float mScale = 0.18f;
    float mRadius = 0.18f * 0.5f;

    int mInstances[64]{};
    RenderInstance mRenderInstances[64]{};

    GLuint mProgram = 0;
    GLuint mVao = 0;
    GLuint mVbo = 0;
    GLuint mEbo = 0;
    GLuint mInstanceVbo = 0;
};

#endif // HOPFIELD_WIDGET