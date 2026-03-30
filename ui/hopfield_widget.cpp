#include <algorithm>
#include <iostream>
#include <random>
#include <string>

#include "hopfield/IHopfieldNetwork.hpp"
#include "helpers/opengl_helpers.hpp"
#include "helpers/file_helpers.hpp"
#include "ui/hopfield_widget.hpp"


HopfieldWidget::HopfieldWidget(IHopfieldNetwork& network, int x, int y, int width, int height)
    : mNetwork(network), mX(x), mY(y), mWidth(width), mHeight(height), mBaseWidth(width), mBaseHeight(height) {

    // initialize render instances from external data
    int idx = 0;
    for (int gy = 0; gy < mNetwork.rows(); ++gy) {
        for (int gx = 0; gx < mNetwork.cols(); ++gx) {
            const float fx = -0.875f + static_cast<float>(gx) * 0.25f;
            const float fy =  0.875f - static_cast<float>(gy) * 0.25f;

            mRenderInstances.push_back(RenderInstance{ fx, fy, 0.0f, 0.0f, 0.0f });
            set_instance_color_from_state(mNetwork.at(gy, gx), mRenderInstances.back());
            ++idx;
        }
    }
}

void HopfieldWidget::setScale(float scale) {
    if (scale <= 0.0f) return;

    mWidth = static_cast<int>(static_cast<float>(mBaseWidth) * scale);
    mHeight = static_cast<int>(static_cast<float>(mBaseHeight) * scale);

    if (mWidth < 1) mWidth = 1;
    if (mHeight < 1) mHeight = 1;

    mScale = mBaseScale * scale;
    mRadius = mScale * 0.5f;
}

void HopfieldWidget::update_render_instances() {
    for (int gy = 0; gy < mNetwork.rows(); ++gy) {
        for (int gx = 0; gx < mNetwork.cols(); ++gx) {
            int idx = gy * mNetwork.cols() + gx;
            set_instance_color_from_state(mNetwork.at(gy, gx), mRenderInstances.at(idx));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mRenderInstances.size() * sizeof(RenderInstance), mRenderInstances.data());
}

void HopfieldWidget::paint_at(double mx, double my) {
    if (mDrawWidth <= 0 || mDrawHeight <= 0) return;
    if (mx < mDrawX || mx >= mDrawX + mDrawWidth || my < mDrawY || my >= mDrawY + mDrawHeight) {
        mLastPaintIdx = -1;
        return;
    }

    const float xNdc = static_cast<float>(((mx - mDrawX) / mDrawWidth) * 2.0 - 1.0);
    const float yNdc = static_cast<float>(1.0 - ((my - mDrawY) / mDrawHeight) * 2.0);

    int bestIdx = -1;
    float bestD2 = mRadius * mRadius;
    for (int i = 0; i < mNetwork.rows() * mNetwork.cols(); ++i) {
        float dx = xNdc - mRenderInstances.at(i).cx;
        float dy = yNdc - mRenderInstances.at(i).cy;
        float d2 = dx*dx + dy*dy;
        if (d2 <= bestD2) {
            bestD2 = d2;
            bestIdx = i;
        }
    }

    if (bestIdx < 0) return;

    if (bestIdx == mLastPaintIdx) return;
    mLastPaintIdx = bestIdx;

    const int row = bestIdx / mNetwork.cols();
    const int col = bestIdx % mNetwork.cols();
    mNetwork.at(row, col) = -mNetwork.at(row, col);
    set_instance_color_from_state(mNetwork.at(row, col), mRenderInstances.at(bestIdx));

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, bestIdx * sizeof(RenderInstance), sizeof(RenderInstance), &mRenderInstances.at(bestIdx));
}

bool HopfieldWidget::init_gl() {
    const std::string vsSrcStr = readTextFile("shaders/hopfield_widget.vert");
    const std::string fsSrcStr = readTextFile("shaders/hopfield_widget.frag");

    if (vsSrcStr.empty() || fsSrcStr.empty())
        return false;

    const char* vsSrc = vsSrcStr.c_str();
    const char* fsSrc = fsSrcStr.c_str();

    mProgram = create_program(vsSrc, fsSrc);
    if (!mProgram) {
        return false;
    } 

    const float quadVertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    const unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    glGenVertexArrays(1, &mVao);
    glGenBuffers(1, &mVbo);
    glGenBuffers(1, &mEbo);
    glGenBuffers(1, &mInstanceVbo);

    glBindVertexArray(mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferData(GL_ARRAY_BUFFER, mRenderInstances.size() * sizeof(RenderInstance), mRenderInstances.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RenderInstance), (void*)0);
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(RenderInstance), (void*)(2 * sizeof(float)));
    glVertexAttribDivisor(2, 1);

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

void HopfieldWidget::draw(int fbWidth, int fbHeight) {
    if (!mProgram || !mVao) return;
    if (mIsDirty) {
        update_render_instances();
        mIsDirty = false;
    }

    if (fbWidth <= 0 || fbHeight <= 0) return;

    const int vx = mX;
    const int vy = fbHeight - (mY + mHeight);

    // The logical widget rect for UI hit testing uses top-left origin.
    mDrawX = mX;
    mDrawY = mY;
    mDrawWidth = mWidth;
    mDrawHeight = mHeight;

    // Viewport is allowed to extend offscreen (to avoid scaling/squishing).
    glViewport(vx, vy, mWidth, mHeight);

    // Scissor must be inside framebuffer; clip to intersection.
    const int sx0 = std::max(vx, 0);
    const int sy0 = std::max(vy, 0);
    const int sx1 = std::min(vx + mWidth, fbWidth);
    const int sy1 = std::min(vy + mHeight, fbHeight);
    const int sw = sx1 - sx0;
    const int sh = sy1 - sy0;
    if (sw <= 0 || sh <= 0) return;

    glEnable(GL_SCISSOR_TEST);
    glScissor(sx0, sy0, sw, sh);

    glUseProgram(mProgram);
    GLint uScale = glGetUniformLocation(mProgram, "uScale");
    glUniform1f(uScale, mScale);

    // Keep circles circular in non-square viewports
    const float aspect = (mHeight != 0) ? (static_cast<float>(mWidth) / static_cast<float>(mHeight)) : 1.0f;
    GLint uAspect = glGetUniformLocation(mProgram, "uAspect");
    glUniform1f(uAspect, aspect);

    glBindVertexArray(mVao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, mNetwork.rows() * mNetwork.cols());
    glBindVertexArray(0);

    glDisable(GL_SCISSOR_TEST);
}

void HopfieldWidget::on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    if (action == GLFW_PRESS) {
        mIsPainting = true;
        mLastPaintIdx = -1;
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        paint_at(mx, my);
    } else if (action == GLFW_RELEASE) {
        mIsPainting = false;
        mLastPaintIdx = -1;
    }
}

void HopfieldWidget::on_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
    if (!mIsPainting) return;
    paint_at(xpos, ypos);
}

void HopfieldWidget::set_instance_color_from_state(int s, RenderInstance& ri) {
    const float on = (s > 0) ? 1.0f : 0.0f;
    const float onR = 0.20f, onG = 0.85f, onB = 0.35f;
    const float offR = 0.15f, offG = 0.17f, offB = 0.20f;
    ri.r = offR + (onR - offR) * on;
    ri.g = offG + (onG - offG) * on;
    ri.b = offB + (onB - offB) * on;
}
