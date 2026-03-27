#include <iostream>
#include <random>
#include <string>

#include "hopfield/hopfield_network.hpp"
#include "helpers/opengl_helpers.hpp"
#include "helpers/file_helpers.hpp"
#include "ui/hopfield_widget.hpp"


HopfieldWidget::HopfieldWidget(HopfieldNetwork& data, int x, int y, int width, int height)
    : mData(data), mX(x), mY(y), mWidth(width), mHeight(height) {

    // initialize render instances from external data
    int idx = 0;
    for (int gy = 0; gy < mData.rows(); ++gy) {
        for (int gx = 0; gx < mData.cols(); ++gx) {
            const float fx = -0.875f + static_cast<float>(gx) * 0.25f;
            const float fy =  0.875f - static_cast<float>(gy) * 0.25f;

            mRenderInstances.push_back(RenderInstance{ fx, fy, 0.0f, 0.0f, 0.0f });
            set_instance_color_from_state(mData.at(gy, gx), mRenderInstances.back());
            ++idx;
        }
    }
}

void HopfieldWidget::update_render_instances() {
    for (int gy = 0; gy < mData.rows(); ++gy) { // todo: is this right?
        for (int gx = 0; gx < mData.cols(); ++gx) {
            int idx = gy * mData.cols() + gx;
            set_instance_color_from_state(mData.at(gy, gx), mRenderInstances.at(idx));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mRenderInstances.size() * sizeof(RenderInstance), mRenderInstances.data());
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

    int vx = mX;
    int vy = fbHeight - (mY + mHeight);
    if (vy < 0) vy = 0;

    glEnable(GL_SCISSOR_TEST);
    glViewport(vx, vy, mWidth, mHeight);
    glScissor(vx, vy, mWidth, mHeight);

    glUseProgram(mProgram);
    GLint uScale = glGetUniformLocation(mProgram, "uScale");
    glUniform1f(uScale, mScale);

    glBindVertexArray(mVao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, mData.rows() * mData.cols());
    glBindVertexArray(0);

    glDisable(GL_SCISSOR_TEST);
}

void HopfieldWidget::on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    if (mx < mX || mx >= mX + mWidth || my < mY || my >= mY + mHeight) return;

    float xNdc = ((mx - mX) / mWidth) * 2.0f - 1.0f;
    float yNdc = 1.0f - ((my - mY) / mHeight) * 2.0f;

    int bestIdx = -1;
    float bestD2 = mRadius * mRadius;
    for (int i = 0; i < mData.rows() * mData.cols(); ++i) {
        float dx = xNdc - mRenderInstances.at(i).cx;
        float dy = yNdc - mRenderInstances.at(i).cy;
        float d2 = dx*dx + dy*dy;
        if (d2 <= bestD2) {
            bestD2 = d2;
            bestIdx = i;
        }
    }

    if (bestIdx < 0) return;

    // flip external data
    int row = bestIdx / mData.cols();
    int col = bestIdx % mData.cols();
    mData.at(row, col) = -mData.at(row, col);

    set_instance_color_from_state(mData.at(row, col), mRenderInstances.at(bestIdx));

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, bestIdx * sizeof(RenderInstance), sizeof(RenderInstance), &mRenderInstances.at(bestIdx));
}

void HopfieldWidget::set_instance_color_from_state(int s, RenderInstance& ri) {
    const float on = (s > 0) ? 1.0f : 0.0f;
    const float onR = 0.20f, onG = 0.85f, onB = 0.35f;
    const float offR = 0.15f, offG = 0.17f, offB = 0.20f;
    ri.r = offR + (onR - offR) * on;
    ri.g = offG + (onG - offG) * on;
    ri.b = offB + (onB - offB) * on;
}
