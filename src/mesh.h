#ifndef MESH_H
#define MESH_H

#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <string>

void load_mesh_from_obj(
    const std::string filename,
    std::vector<glm::vec3>& verts,
    std::vector<glm::vec3>& normals,
    std::vector<glm::u32vec3>& tris
);

void get_edges_from_mesh(
    const std::vector<glm::u32vec3>& tris,
    std::vector<glm::u32vec2>& edges,
    std::vector<glm::u32vec2>& adjacent
);

#endif // MESH_H