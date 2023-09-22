#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

/// Struct for handling the material for the linerender
/// where:
/// - `int_colour` is the colour of the interior lines.
/// - `ext_colour` is the colour of exterior edges.
struct material {
    glm::vec3 int_colour;
    glm::vec3 ext_colour;

    material(const glm::vec3& interior, const glm::vec3& exterior);

    material(const material& other) = default;
};

#endif