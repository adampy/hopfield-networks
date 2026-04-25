#include "ui/HopfieldWidget.hpp"

#include <algorithm>
#include <iostream>
#include <string>

#include "helpers/FileHelpers.hpp"
#include "helpers/OpenGLHelpers.hpp"

HopfieldWidget::HopfieldWidget(IHopfieldNetwork& network, int x, int y, int width, int height)
    : mNetwork(network), mX(x), mY(y), mWidth(width), mHeight(height), mBaseWidth(width), mBaseHeight(height) {
    rebuild_instances();
}

void HopfieldWidget::rebuild_instances() {
    mRenderInstances.clear();
    mRenderInstances.reserve(static_cast<size_t>(mNetwork.rows() * mNetwork.cols()));
    for (int r = 0; r < mNetwork.rows(); ++r) {
        for (int c = 0; c < mNetwork.cols(); ++c) {
            const float state = (mNetwork.at(r, c) > 0) ? 1.0f : 0.0f;
            mRenderInstances.push_back(RenderInstance{ static_cast<float>(c), static_cast<float>(r), state });
        }
    }
}

void HopfieldWidget::paint_at(double mx, double my, int targetValue) {
    if (mDrawWidth <= 0 || mDrawHeight <= 0) return;
    if (mx < mDrawX || mx >= mDrawX + mDrawWidth || my < mDrawY || my >= mDrawY + mDrawHeight) {
        mLastPaintIdx = -1;
        return;
    }

    const double fx = (mx - mDrawX) / static_cast<double>(mDrawWidth);
    const double fy = (my - mDrawY) / static_cast<double>(mDrawHeight);

    const int col = static_cast<int>(fx * mNetwork.cols());
    // GLFW cursor Y is top->bottom but grid/shader row 0 is at bottom
    const int row = (mNetwork.rows() - 1) - static_cast<int>(fy * mNetwork.rows());

    if (row < 0 || row >= mNetwork.rows() || col < 0 || col >= mNetwork.cols()) return;

    const int idx = row * mNetwork.cols() + col;
    if (idx == mLastPaintIdx) return;
    mLastPaintIdx = idx;

    if (targetValue >= 0) {
        mNetwork.at(row, col) = 1;
    } else {
        mNetwork.at(row, col) = -1;
    }

    const float state = (mNetwork.at(row, col) > 0) ? 1.0f : 0.0f;
    mRenderInstances.at(idx).state = state;

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, idx * sizeof(RenderInstance), sizeof(RenderInstance), &mRenderInstances.at(idx));
}

bool HopfieldWidget::init_gl() {
    // Create shaders and return false if errors
    const std::string vsSrcStr = readTextFile("shaders/hopfield_widget.vert");
    const std::string fsSrcStr = readTextFile("shaders/hopfield_widget.frag");
    if (vsSrcStr.empty() || fsSrcStr.empty()) return false;
    mProgram = createProgram(vsSrcStr.c_str(), fsSrcStr.c_str());
    if (!mProgram) return false;

    // Define unit quad geometry
    const float quadVertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    const unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    // Create GPU objects:
    // - VAO: vertex input state (which buffers feed which attributes)
    // - VBO: per-vertex quad geometry (aPos)
    // - EBO: quad indices (two triangles)
    // - instance VBO: per-cell data (iCell, iState)
    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);
    glGenBuffers(1, &mInstanceVbo);

    // Bind the VAO first so all subsequent vertex attribute/buffer bindings are recorded into it.
    glBindVertexArray(mVao);

    // Upload unit quad geometry
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Upload quad indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // aPos (per-vertex, 2 floats)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // Upload per-cell instance data
    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferData(GL_ARRAY_BUFFER, mRenderInstances.size() * sizeof(RenderInstance), mRenderInstances.data(), GL_DYNAMIC_DRAW);

    // iCell (per-instance, 2 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RenderInstance), (void*)0);
    glVertexAttribDivisor(1, 1);

    // iState (per-instance, 1 float)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(RenderInstance), (void*)(2 * sizeof(float)));
    glVertexAttribDivisor(2, 1);

    // Unbind VAO
    glBindVertexArray(0);

    return true;
}

void HopfieldWidget::shutdown_gl() {
    if (mVao) glDeleteVertexArrays(1, &mVao);
    if (mVbo) glDeleteBuffers(1, &mVbo);
    if (mEbo) glDeleteBuffers(1, &mEbo);
    if (mInstanceVbo) glDeleteBuffers(1, &mInstanceVbo);
    if (mProgram) glDeleteProgram(mProgram);
    mVao = 0;
    mVbo = 0;
    mEbo = 0;
    mInstanceVbo = 0;
    mProgram = 0;
}

void HopfieldWidget::setScale(float scale) {
    if (scale <= 0.0f) return;
    mScale = scale;

    mWidth = static_cast<int>(static_cast<float>(mBaseWidth) * scale);
    mHeight = static_cast<int>(static_cast<float>(mBaseHeight) * scale);

    if (mWidth < 1) mWidth = 1;
    if (mHeight < 1) mHeight = 1;
}

void HopfieldWidget::draw(int fbWidth, int fbHeight) {
    // Early return if cannot draw
    if (fbWidth <= 0 || fbHeight <= 0) return;
    if (!mProgram || !mVao) return;

    if (mIsDirty) {
        // then need to rebuild render instances
        rebuild_instances();
        glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, mRenderInstances.size() * sizeof(RenderInstance), mRenderInstances.data());
        mIsDirty = false;
    }    

    const int vx = mX;
    const int vy = fbHeight - (mY + mHeight);

    mDrawX = mX;
    mDrawY = mY;
    mDrawWidth = mWidth;
    mDrawHeight = mHeight;

    // Set widget viewport in framebuffer space (origin is bottom-left for OpenGL)
    glViewport(vx, vy, mWidth, mHeight);

    // Compute the visible portion of the widget (clip to framebuffer bounds)
    const int sx0 = std::max(vx, 0);
    const int sy0 = std::max(vy, 0);
    const int sx1 = std::min(vx + mWidth, fbWidth);
    const int sy1 = std::min(vy + mHeight, fbHeight);
    const int sw = sx1 - sx0;
    const int sh = sy1 - sy0;
    if (sw <= 0 || sh <= 0) return;

    // Scissor to visible region (so don't draw outside framebuffer)
    glEnable(GL_SCISSOR_TEST);
    glScissor(sx0, sy0, sw, sh);

    // Bind shader program and update grid dimensions
    glUseProgram(mProgram);
    GLint uGrid = glGetUniformLocation(mProgram, "uGrid");
    glUniform2f(uGrid, static_cast<float>(mNetwork.cols()), static_cast<float>(mNetwork.rows()));

    // Draw indexed quad once per neuron using instancing
    glBindVertexArray(mVao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, mNetwork.rows() * mNetwork.cols());
    glBindVertexArray(0);

    // Restore state
    glDisable(GL_SCISSOR_TEST);
}

void HopfieldWidget::on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT && button != GLFW_MOUSE_BUTTON_RIGHT) return;

    if (action == GLFW_PRESS) {
        mIsPainting = true;
        mPaintValue = (button == GLFW_MOUSE_BUTTON_LEFT) ? 1 : -1;
        mLastPaintIdx = -1;
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        paint_at(mx, my, mPaintValue);
    } else if (action == GLFW_RELEASE) {
        mIsPainting = false;
        mLastPaintIdx = -1;
    }
}

void HopfieldWidget::on_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
    if (!mIsPainting) return;
    paint_at(xpos, ypos, mPaintValue);
}
