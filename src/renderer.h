#ifndef RENDERER_H
#define RENDERER_H

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include <vector>

/// A renderer for 2d lines and 2d lines only, I specifically avoid
/// using gl functions to transform the lines or cull them in any way
/// as I want to write a strictly cpu renderer.
///
/// Must be initialized striclty after the renderer.
class renderer {
    GLuint shader_prog = NULL;
    GLuint v_shader = NULL, f_shader = NULL;
    GLuint col_map = NULL;

    GLuint vao = NULL;
    GLuint bufs[2] = { NULL, NULL };

    GLint tex_loc, csize_loc, aratio_loc;

    void clean_up();
public: 
    renderer();
    ~renderer();

    /// compiles the shader program and initializes the renderer.
    /// should only be called after gl is initalized.
    ///
    /// made separate from the ctor in order to not be accidentally 
    /// initalized before gl.
    ///
    /// returns:
    ///     0, on success.
    ///     -1, on failiure.
    int init();

    /// draws the lines dicated by the vertices V and the indices into 
    /// them VI.
    ///
    /// Line i is drawn with color C.
    ///
    /// Copies data with every draw call to the gpu (terrible for performance, 
    /// but I don't really care for the purposes of what I'm creating here).
    void draw(
        const std::vector<glm::vec2>& V, 
        const std::vector<GLuint>& VI, 
        const std::vector<glm::vec3>& C,
        const GLfloat aspect_ratio
    );
};

#endif