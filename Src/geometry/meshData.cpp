#include <vector>
#include <geometry/meshData.h>

void MeshData::GenerateTangents() {
        constexpr int STRIDE = 8;
        std::vector tangents(vertices.size() / STRIDE, glm::vec3(0.0f));

        for (size_t i = 0; i < indices.size(); i+=3) {
                uint32_t i0 = indices[i];
                uint32_t i1 = indices[i+1];
                uint32_t i2 = indices[i+2];

                auto p0 = VecFromVertices<3>(STRIDE, i0);
                auto p1 = VecFromVertices<3>(STRIDE, i1);
                auto p2 = VecFromVertices<3>(STRIDE, i2);

                auto uv0 = VecFromVertices<2>(STRIDE, i0, 6);
                auto uv1 = VecFromVertices<2>(STRIDE, i1, 6);
                auto uv2 = VecFromVertices<2>(STRIDE, i2, 6);

                const glm::vec3 edge1 = p1 - p0;
                const glm::vec3 edge2 = p2 - p0;
                const glm::vec2 deltaUV1 = uv1 - uv0;
                const glm::vec2 deltaUV2 = uv2 - uv0;

                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                glm::vec3 tangent;
                tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

                tangents[i0] += tangent;
                tangents[i1] += tangent;
                tangents[i2] += tangent;
        }

        std::vector<float> newVertices;
        newVertices.reserve((vertices.size() / STRIDE) * 11);

        for (size_t i = 0; i < vertices.size() / STRIDE; ++i)
        {
                for (int j = 0; j < STRIDE; ++j)
                        newVertices.push_back(vertices[i * STRIDE + j]);

                glm::vec3 t = glm::normalize(tangents[i]);
                newVertices.push_back(t.x);
                newVertices.push_back(t.y);
                newVertices.push_back(t.z);
        }

        verticesWithTangents = std::move(newVertices);
}
