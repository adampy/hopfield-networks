#ifndef OPENGL_HELPERS
#define OPENGL_HELPERS

#include <glad/glad.h>

/**
 * Helper function for compiling a shader from source code.
 * Returns the shader ID, or 0 if compilation fails.
 */
static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(static_cast<size_t>(logLen), '\0');
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        std::cerr << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

/**
 * Compiles vertex and fragment shaders from source code and links them into a program.
 * Returns the program ID, or 0 if compilation or linking fails.
 */
static GLuint createProgram(const char* vsSrc, const char* fsSrc) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
    if (!vs) {
        return 0;
    }
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    if (!fs) {
        glDeleteShader(vs);
        return 0;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(static_cast<size_t>(logLen), '\0');
        glGetProgramInfoLog(prog, logLen, nullptr, log.data());
        std::cerr << log << "\n";
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

#endif // OPENGL_HELPERS