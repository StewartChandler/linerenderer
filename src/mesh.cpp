#include "mesh.h"

#include <fstream>
#include <sstream>
#include <stdint.h>
#include <unordered_map>

typedef uint32_t u32;
typedef uint64_t u64;

void load_mesh_from_obj(
    const std::string filename,
    std::vector<glm::vec3>& verts,
    std::vector<glm::vec3>& normals,
    std::vector<glm::u32vec3>& tris
) {
    std::ifstream file(filename);
    
    if (!file.is_open()) return;

    u32 v_offset = (u32) verts.size();
    assert(normals.size() == tris.size());

    std::vector<glm::vec3> vn; // vertex normals

    std::string line, tok;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        
        iss >> tok;

        if (tok == "v") {
            glm::vec3 v;  
            iss >> v.x >> v.y >> v.z;

            verts.push_back(v);
        } else if (tok == "vn") {
            glm::vec3 v;
            iss >> v.x >> v.y >> v.z;

            vn.push_back(v);
        } else if (tok == "f") {
            glm::vec3 n {0.0f, 0.0f, 0.0f};
            glm::u32vec3 t;

            for (int i = 0; i < 3; i++) {
                iss >> tok;
                t[i] = std::stoi(tok.substr(0, tok.find_first_of('/'))) - 1 + v_offset;
                u32 ni = std::stoi(tok.substr(tok.find_last_of('/') + 1, std::string::npos)) - 1;

                n += vn[ni];
            }
            
            normals.push_back(glm::normalize(n));
            tris.push_back(t);
        }
    }
    
    file.close();
}

struct edge_hasher {
    std::size_t operator()(glm::u32vec2 const& e) const noexcept {
        return std::hash<u64>{}((((u64) e.x) << 32) | (e.y));
    }
};

void get_edges_from_mesh(
    const std::vector<glm::u32vec3>& tris,
    std::vector<glm::u32vec2>& edges,
    std::vector<glm::u32vec2>& adjacent
) {
    std::unordered_map<glm::u32vec2, u32, edge_hasher> e_idx;
    assert(edges.size() == adjacent.size());

    for (int i = 0; i < tris.size(); i++) {
        const glm::u32vec3 &t = tris[i];
        for (int j = 0; j < 3; j++) {
            glm::u32vec2 edge {t[j], t[(j + 1) % 3]};
            if (edge.y > edge.x) std::swap(edge.x, edge.y);

            u32 index;
            if (auto search = e_idx.find(edge); search != e_idx.end()) {
                index = search->second;
                if (adjacent[index].y == (u32) -1) {
                    adjacent[index].y = i;
                }
            } else {
                index = (u32) edges.size();
                edges.push_back(edge);
                adjacent.emplace_back(i, (u32) -1);
                e_idx.emplace(edge, index);
            }
        }
    }
}
