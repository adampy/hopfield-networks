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

    void setDirty() { mIsDirty = true; }
    void setScale(float scale);

private:
    struct RenderInstance {
        float ix;
        float iy;
        float state;
    };

    void rebuild_instances();
    void paint_at(double mx, double my, int targetValue);

    IHopfieldNetwork& mNetwork;

    // Widget position and size
    int mX, mY, mWidth, mHeight;
    int mBaseWidth, mBaseHeight;
    float mScale = 1.0f;

    // Drawing area within the widget
    int mDrawX = 0, mDrawY = 0, mDrawWidth = 0, mDrawHeight = 0;

    // OpenGL-related members
    GLuint mProgram = 0;
    GLuint mVao = 0;
    GLuint mVbo = 0;
    GLuint mEbo = 0;
    GLuint mInstanceVbo = 0;
    
    // State
    std::vector<RenderInstance> mRenderInstances = {};
    bool mIsDirty = true;
    bool mIsPainting = false;
    int mPaintValue = 1;
    int mLastPaintIdx = -1;
};

#endif // HOPFIELD_WIDGET
