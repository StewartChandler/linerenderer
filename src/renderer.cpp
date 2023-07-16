#include "renderer.h"
#include <string>
#include <iostream>

const GLchar* vert_src = R"glsl(
    #version 150

    uniform float aRatio;
    in vec3 pos;

    void main() {
        gl_Position = vec4(pos.x / aRatio, pos.y, pos.z, 1.0);
    }
)glsl"; 

const GLchar* frag_src = R"glsl(
    #version 150

    uniform sampler1D colMap;
    uniform float csize;

    out vec4 fragColour;

    void main() {
        float t = (float(gl_PrimitiveID) + 0.5) / csize;
        fragColour = texture(colMap, t);
    }
)glsl"; 

static std::string getShaderInfoLog(GLuint shader) {
    std::string res;
    GLint length;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    if (length == 0) {
        return res;
    }

    res.resize(length);
    glGetShaderInfoLog(shader, length, NULL, res.data());

    return res;
}

static std::string getProgramInfoLog(GLuint program) {
    std::string res;
    GLint length;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    if (length == 0) {
        return res;
    }

    res.resize(length);
    glGetProgramInfoLog(program, length, NULL, res.data());

    return res;
}

void renderer::clean_up() {
    if (shader_prog != NULL) { // already been initalized, should reinit 
        glDeleteProgram(shader_prog);
        shader_prog = NULL;
    }

    if (v_shader != NULL) {
        glDeleteShader(v_shader);
        v_shader = NULL;
    }

    if (f_shader != NULL) {
        glDeleteShader(f_shader);
        f_shader = NULL;
    }

    if (vao != NULL) {
        glDeleteVertexArrays(1, &vao);
        vao = NULL;
    }

    for (auto &buf : bufs) {
        if (buf != NULL) {
            glDeleteBuffers(1, &buf);
        }
    }

    if (col_map != NULL) {
        glDeleteTextures(1, &col_map);
    }
}

renderer::renderer() {};

renderer::~renderer() {
    clean_up();
}


int renderer::init() {
    clean_up();

    v_shader = glCreateShader(GL_VERTEX_SHADER);
    f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(v_shader, 1, &vert_src, NULL);
    glShaderSource(f_shader, 1, &frag_src, NULL);

    glCompileShader(v_shader);
    glCompileShader(f_shader);

    GLint v_compiled, f_compiled;
    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &v_compiled);
    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &f_compiled);

    // std::cout << "v_compiled: " << v_compiled << "\n";
    // std::cout << "f_compiled: " << f_compiled << "\n";

    if (v_compiled != GL_TRUE) {
        std::string err = getShaderInfoLog(v_shader);
        std::cerr << "Vertex shader failed to compile with:\n" << err << "\n"; 
    }

    if (f_compiled != GL_TRUE) {
        std::string err = getShaderInfoLog(f_shader);
        std::cerr << "Fragment shader failed to compile with:\n" << err << "\n"; 
    }

    if (v_compiled != GL_TRUE || f_compiled != GL_TRUE) {
        glDeleteShader(v_shader);
        glDeleteShader(f_shader);

        v_shader = NULL;
        f_shader = NULL;
        return -1;
    }

    shader_prog = glCreateProgram();

    glAttachShader(shader_prog, v_shader);
    glAttachShader(shader_prog, f_shader);

    glBindFragDataLocation(shader_prog, 0, "fragColour");

    glLinkProgram(shader_prog);

    GLint progLinked;
    glGetProgramiv(shader_prog, GL_LINK_STATUS, &progLinked);

    if (progLinked != GL_TRUE) {
        std::string err = getProgramInfoLog(shader_prog);
        std::cerr << err << "\n";

        glDeleteProgram(shader_prog);

        shader_prog = NULL;

        glDeleteShader(v_shader);
        glDeleteShader(f_shader);

        v_shader = NULL;
        f_shader = NULL;
        return -1;
    }

    const GLuint &vbo = bufs[0], &ebo = bufs[1];

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, bufs);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    GLint pos_attrib = glGetAttribLocation(shader_prog, "pos");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    tex_loc = glGetUniformLocation(shader_prog, "colMap");
    csize_loc = glGetUniformLocation(shader_prog, "csize");
    aratio_loc = glGetUniformLocation(shader_prog, "aRatio");

    glGenTextures(1, &col_map);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, col_map);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return 0;
};

void renderer::draw(
    const std::vector<glm::vec3>& V, 
    const std::vector<glm::u32vec2>& VI, 
    const std::vector<glm::vec3>& C,
    const GLfloat aspect_ratio
) {
    const GLuint &vbo = bufs[0], &ebo = bufs[1];

    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, col_map);

    static_assert(sizeof(glm::vec3) == sizeof(GLfloat[3]), "glm::vec3 must be tightly packed GLfloat array");
    GLsizeiptr vert_arr_size = V.size() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, vert_arr_size, (const GLfloat*) V.data(), GL_STREAM_DRAW);

    static_assert(sizeof(glm::u32vec2) == sizeof(GLuint[2]), "glm::u32vec2 must be tightly packed GLuint array");
    GLsizeiptr elem_arr_size = VI.size() * sizeof(glm::u32vec2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elem_arr_size, (const GLuint*) VI.data(), GL_STREAM_DRAW);

    static_assert(sizeof(glm::vec3) == sizeof(GLfloat[3]), "glm::vec3 must be tightly packed GLfloat array");
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, (GLsizei) C.size(), 0, GL_RGB, GL_FLOAT, C.data());

    glUseProgram(shader_prog);
    glUniform1i(tex_loc, 0);
    glUniform1f(csize_loc, (GLfloat) C.size());
    glUniform1f(aratio_loc, (GLfloat) aspect_ratio);

    glDrawElements(GL_LINES, (GLsizei) VI.size(), GL_UNSIGNED_INT, 0);
}