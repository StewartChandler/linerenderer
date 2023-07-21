#ifndef CAMERA_H
#define CAMERA_H

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

class camera {
public:
    glm::mat4 proj;
    glm::mat4 view;

    camera(const glm::mat4& projection, const glm::mat4& transform); 
    
    void transform(
        const glm::mat4& model,
        const std::vector<glm::vec3>& verts,
        const std::vector<glm::vec3>& norms,
        std::vector<glm::vec3>& t_verts,
        std::vector<glm::vec3>& t_norms
    );
};

#endif
