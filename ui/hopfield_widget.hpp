#ifndef HOPFIELD_WIDGET
#define HOPFIELD_WIDGET

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "hopfield/IHopfieldNetwork.hpp"

/**
 * The UI widget in which a Hopfield network can be displayed.
 * 
 * @author Adam Crawley
 */
class HopfieldWidget {
public:
    HopfieldWidget(IHopfieldNetwork& network, int x, int y, int width, int height);

    bool init_gl();
    void shutdown_gl();
    void draw(int fbWidth, int fbHeight);
    void on_mouse_button(GLFWwindow* window, int button, int action, int mods);
    void on_cursor_pos(GLFWwindow* window, double xpos, double ypos);
    void setDirty() { mIsDirty = true; };
    void setScale(float scale);

private:
    struct RenderInstance {
        float cx, cy;
        float r, g, b;
    };

    void update_render_instances();
    void paint_at(double mx, double my);

    static void set_instance_color_from_state(int s, RenderInstance& ri);

    IHopfieldNetwork& mNetwork;

    int mX, mY, mWidth, mHeight;
    int mBaseWidth, mBaseHeight;
    int mDrawX = 0, mDrawY = 0, mDrawWidth = 0, mDrawHeight = 0;
    float mBaseScale = 0.18f;
    float mScale = 0.18f;
    float mRadius = 0.18f * 0.5f;

    std::vector<RenderInstance> mRenderInstances = {};

    GLuint mProgram = 0;
    GLuint mVao = 0;
    GLuint mVbo = 0;
    GLuint mEbo = 0;
    GLuint mInstanceVbo = 0;

    bool mIsDirty = true; // boolean as to whether the widget needs to be fully redrawn
    bool mIsPainting = false;
    int mLastPaintIdx = -1;
};

#endif // HOPFIELD_WIDGET