#include "camera.h"

camera::camera(const glm::mat4& projection, const glm::mat4& transform) :
    proj(projection), 
    view(transform) {}

void camera::transform(
    const glm::mat4& model,
    const std::vector<glm::vec3>& verts,
    const std::vector<glm::vec3>& norms,
    std::vector<glm::vec3>& t_verts,
    std::vector<glm::vec3>& t_norms
) {
    glm::mat4 modelviewproj = view * model;
    glm::mat3 normal_mat = glm::transpose(glm::inverse(glm::mat3(modelviewproj)));
    modelviewproj = proj * modelviewproj;

    t_verts.resize(verts.size());
    t_norms.resize(norms.size());

    for (int i = 0; i < norms.size(); i++) {
        t_norms[i] = glm::normalize(normal_mat * norms[i]);
    }

    for (int i = 0; i < verts.size(); i++) {
        glm::vec4 temp = modelviewproj * glm::vec4(verts[i], 1.0f);
        
        t_verts[i] = glm::vec3(temp) / temp.w;
    }
}
