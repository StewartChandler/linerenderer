#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>

#include "GL/glcorearb.h"
#include "camera.h"
#include "renderer.h"
#include "mesh.h"

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
        // test loading of meshes
        std::vector<glm::vec3> verts, norms;
        std::vector<glm::u32vec3> tris;
        load_mesh_from_obj("res/monkey2.obj", verts, norms, tris);
        // std::cout << "Vertices:\n";
        // for (auto& v : verts) {
        //     std::cout << "  " << v.x << ", " << v.y << ", " << v.z << "\n";
        // }
        // std::cout << "Normals:\n";
        // for (auto& n : norms) {
        //     std::cout << "  " << n.x << ", " << n.y << ", " << n.z << "\n";
        // }
        // std::cout << "Triangles:\n";
        // for (auto& t : tris) {
        //     std::cout << "  " << t.x << ", " << t.y << ", " << t.z << "\n";
        // }
        
        std::vector<glm::u32vec2> edges, adj;
        get_edges_from_mesh(tris, edges, adj);
        // std::cout << "Edges:\n";
        // for (auto& e : edges) {
        //     std::cout << "  " << e.x << ", " << e.y << "\n";
        // }
        // std::cout << "Adj:\n";
        // for (auto& f : adj) {
        //     std::cout << "  " << f.x << ", " << f.y << "\n";
        // }

        std::vector<glm::vec3> colour(edges.size(), glm::vec3(0.8f, 0.7f, 0.5f));
        
        int width, height;
        glfwGetWindowSize(w, &width, &height);

        std::unique_ptr<camera> cam = std::make_unique<camera>(
            glm::infinitePerspective(1.5f, width / (float) height, 1.0f),
            glm::lookAt(
                glm::vec3(0.0f, 2.5f, -4.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            )
        );

        std::vector<glm::vec3> t_verts, t_norms;
        cam->transform(
            glm::identity<glm::mat4>(),
            verts,
            norms,
            t_verts,
            t_norms
        );

        std::vector<glm::u32vec2> hi_edges, lo_edges;
        cam->cull_edges(edges, adj, t_verts, t_norms, hi_edges, lo_edges);
        
        // std::cout << "Transformed Vertices:\n";
        // for (auto& v : t_verts) {
        //     std::cout << "  " << v.x << ", " << v.y << ", " << v.z << "\n";
        // }
        // std::cout << "Transformed Normals:\n";
        // for (auto& n : t_norms) {
        //     std::cout << "  " << n.x << ", " << n.y << ", " << n.z << "\n";
        // }
        
        renderer ren;
        if (ren.init() != 0) {
            glfwDestroyWindow(w);
            glfwTerminate();
            return -1;
        }

        while(!glfwWindowShouldClose(w)) {
            glClear(GL_COLOR_BUFFER_BIT);

            ren.draw(t_verts, edges, colour);

            glfwSwapBuffers(w);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(w);
    glfwTerminate();

    return 0;
}