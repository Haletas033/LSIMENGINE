#ifndef LSIM_MESHDATA_H
#define LSIM_MESHDATA_H
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

struct MeshData {
    std::vector<float> vertices;
    std::vector<float> verticesWithTangents;
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
