#include <iostream>
#include <random>
#include <string>

#include "helpers/opengl_helpers.hpp"
#include "helpers/file_helpers.hpp"
#include "ui/hopfield_widget.hpp"


HopfieldWidget::HopfieldWidget(int rows, int cols, int x, int y, int width, int height)
    : mRows(rows), mCols(cols), mX(x), mY(y),
    mWidth(width), mHeight(height) {

    int idx = 0;
    for (int gy = 0; gy < mRows; ++gy) {
        for (int gx = 0; gx < mCols; ++gx) {
            const float fx = -0.875f + static_cast<float>(gx) * 0.25f;
            const float fy =  0.875f - static_cast<float>(gy) * 0.25f;
            mInstances[idx] = -1;
            mRenderInstances[idx] = RenderInstance{ fx, fy, 0.0f, 0.0f, 0.0f };
            set_instance_color_from_state(-1, mRenderInstances[idx]);
            ++idx;
        }
    }
}

bool HopfieldWidget::init_gl() {
    const std::string vsSrcStr = read_text_file("shaders/hopfield_widget.vert");
    const std::string fsSrcStr = read_text_file("shaders/hopfield_widget.frag");

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(mRenderInstances), mRenderInstances, GL_STATIC_DRAW);

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
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0, mRows * mCols);
    glBindVertexArray(0);

    glDisable(GL_SCISSOR_TEST);
}

void HopfieldWidget::on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    double mx = 0.0, my = 0.0;
    glfwGetCursorPos(window, &mx, &my);

    if (mx < static_cast<double>(mX) || mx >= static_cast<double>(mX + mWidth) ||
        my < static_cast<double>(mY) || my >= static_cast<double>(mY + mHeight))
        return;

    const float xNdc = static_cast<float>(((mx - static_cast<double>(mX)) / static_cast<double>(mWidth)) * 2.0 - 1.0);
    const float yNdc = static_cast<float>(1.0 - ((my - static_cast<double>(mY)) / static_cast<double>(mHeight)) * 2.0);

    int bestIdx = -1;
    float bestD2 = mRadius * mRadius;
    for (int i = 0; i < mRows * mCols; ++i) {
        const float dx = xNdc - mRenderInstances[i].cx;
        const float dy = yNdc - mRenderInstances[i].cy;
        const float d2 = dx * dx + dy * dy;
        if (d2 <= bestD2) {
            bestD2 = d2;
            bestIdx = i;
        }
    }

    if (bestIdx < 0) return;

    mInstances[bestIdx] = -mInstances[bestIdx];
    set_instance_color_from_state(mInstances[bestIdx], mRenderInstances[bestIdx]);

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceVbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        static_cast<GLintptr>(bestIdx * static_cast<int>(sizeof(RenderInstance))),
        static_cast<GLsizeiptr>(sizeof(RenderInstance)),
        &mRenderInstances[bestIdx]);
}

void HopfieldWidget::set_instance_color_from_state(int s, RenderInstance& ri) {
    const float on = (s > 0) ? 1.0f : 0.0f;
    const float onR = 0.20f, onG = 0.85f, onB = 0.35f;
    const float offR = 0.15f, offG = 0.17f, offB = 0.20f;
    ri.r = offR + (onR - offR) * on;
    ri.g = offG + (onG - offG) * on;
    ri.b = offB + (onB - offB) * on;
}
