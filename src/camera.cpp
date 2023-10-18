#include "camera.h"
#include "glm/matrix.hpp"
#include <iterator>
#include <type_traits>
#include <vector>
#include <algorithm>

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

void camera::cull_edges (
    const std::vector<glm::u32vec2>& edges,
    const std::vector<glm::u32vec2>& adj,
    const std::vector<glm::vec3>& verts,
    const std::vector<glm::vec3>& norms,
    std::vector<glm::u32vec2>& hi,
    std::vector<glm::u32vec2>& lo
) {
    assert(edges.size() == adj.size());

    static std::vector<glm::vec3> vert_dir;
    vert_dir.clear();
    vert_dir.resize(verts.size());

    hi.clear();
    lo.clear();

    // inverse of rotation mat
    glm::mat3 irot = glm::transpose(glm::mat3{view});
    std::transform(
        std::begin(verts),
        std::end(verts), 
        std::begin(vert_dir), 
        [&](const glm::vec3& vert) {
            // incorrect
            // TODO: make inverse projection 
            return irot * vert;
        }
    );

    for (size_t i = 0; i < edges.size(); ++i) {
        const auto& e = edges[i];
        const auto& a = adj[i];

        // TODO cull edges, fix errant verts

        if (a[0] != -1) {
            const auto& d1 = vert_dir[e[0]];
            const auto& d2 = vert_dir[e[1]];
            const auto& n1 = norms[a[0]];
            const auto& n2 = norms[a[1]];

            const bool facing11 =  glm::dot(d1, n1) > 0.0f;
            const bool facing12 =  glm::dot(d2, n1) > 0.0f;
            const bool facing21 =  glm::dot(d1, n2) > 0.0f;
            const bool facing22 =  glm::dot(d2, n2) > 0.0f;
            if (facing11 && facing12 && facing21 && facing22) {
                // is an interior edge
                if (glm::dot(n1, n2) < 0.99f) {
                    // cull edges that are flush with the renderer
                    lo.push_back(e);
                }
            } else if ((facing11 && facing12) || (facing21 && facing22)) {
                // is visual edge, render
                hi.push_back(e);
            } // else: is back edge, don't render
        } else { 
            // is mesh edge
            hi.push_back(e);
        }
    }
}
