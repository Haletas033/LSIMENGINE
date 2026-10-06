#ifndef LSIM_MESHDATA_H
#define LSIM_MESHDATA_H
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 uv;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    void GenerateTangents();
};

#endif //LSIM_MESHDATA_H
