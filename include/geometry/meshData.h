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

private:
    // Helper to generate the vecs needed for tangents
    template <glm::length_t N>
    using vec = glm::vec<N, float>;
    template <glm::length_t N>
    vec<N> VecFromVertices(const int STRIDE, const uint32_t iN, const int offset = 0) {
        vec<N> p;

        for (int i = 0; i < N; i++) {
            p[i] = vertices[iN*STRIDE+i+offset];
        }

        return p;
    }
};

#endif //LSIM_MESHDATA_H
