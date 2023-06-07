#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <iomanip>
#include <vector>

#include "GL/glcorearb.h"
#include "renderer.h"

const GLuint ignored_ids[] = {
    0x00020071u
};

void GLAPIENTRY MessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    std::cerr << "[GL Callback] "
        << "[Source: 0x" << std::setw(8) << std::setfill('0') << std::hex << source << "] "
        << "[Type: 0x" << std::setw(8) << std::setfill('0') << std::hex << type << "] "
        << "[ID: 0x" << std::setw(8) << std::setfill('0') << std::hex << id << "] "
        << "[Severity: 0x" << std::setw(8) << std::setfill('0') << std::hex << severity << "] "
        << "Message:\n  " << message << "\n";
}

void glfwCallback(int error, const char* desc) {
    std::cerr << "[GLFW Callback] "
        << "[Type: 0x" << std::setw(8) << std::setfill('0') << std::hex << error << "] "
        << "Message:\n  " << desc << "\n";
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(glfwCallback);

    if (!glfwInit()) {
        std::cout << "failed to init GLFW\n";
        return 1;
    }

    GLFWwindow* w = glfwCreateWindow(1280, 720, "test window", NULL, NULL);

    if (!w) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(w);

    if (gl3wInit()) {
        glfwDestroyWindow(w);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    // ignore messages in the ignored ids list.
    glDebugMessageControl(
        GL_DEBUG_SOURCE_API, 
        GL_DEBUG_TYPE_OTHER, 
        GL_DONT_CARE, 
        sizeof(ignored_ids) / sizeof(GLuint), 
        ignored_ids, 
        GL_FALSE
    );

    {
        renderer ren;
        if (ren.init() != 0) {
            glfwDestroyWindow(w);
            glfwTerminate();
            return -1;
        }

        const std::vector<glm::vec2> V {
            {-0.5f, -0.5f}, // (X, Y)
            {-0.5f,  0.5f},
            { 0.5f, -0.5f},
            { 0.5f,  0.5f}
        };

        const std::vector<GLuint> VI {
            0, 1, // (P1, P2)
            1, 3,
            3, 2,
            2, 0
        };

        const std::vector<glm::vec3> C {
            {0.8f, 0.7f, 0.5f},
            {0.5f, 0.8f, 0.7f},
            {0.7f, 0.5f, 0.8f},
            {0.8f, 0.5f, 0.7f}
        };

        while(!glfwWindowShouldClose(w)) {
            int width, height;
            glfwGetWindowSize(w, &width, &height);

            glClear(GL_COLOR_BUFFER_BIT);

            ren.draw(V, VI, C, width / (float) height);

            glfwSwapBuffers(w);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(w);
    glfwTerminate();

    return 0;
}