#include <vector>
#include <geometry/meshData.h>
#include <glm/geometric.hpp>

void MeshData::GenerateTangents() {
        std::vector tangents(
                vertices.size(),
                glm::vec3(0.0f)
        );

        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                const uint32_t i0 = indices[i];
                const uint32_t i1 = indices[i + 1];
                const uint32_t i2 = indices[i + 2];

                const glm::vec3 edge1 = vertices[i1].position - vertices[i0].position;

                const glm::vec3 edge2 = vertices[i2].position - vertices[i0].position;

                const glm::vec2 deltaUV1 = vertices[i1].uv - vertices[i0].uv;

                const glm::vec2 deltaUV2 = vertices[i2].uv - vertices[i0].uv;

                const float denominator =
                        deltaUV1.x * deltaUV2.y -
                        deltaUV2.x * deltaUV1.y;

                if (denominator == 0.0f) continue;

                const float f = 1.0f / denominator;

                glm::vec3 tangent;
                tangent.x = f * (
                        deltaUV2.y * edge1.x -
                        deltaUV1.y * edge2.x
                );

                tangent.y = f * (
                        deltaUV2.y * edge1.y -
                        deltaUV1.y * edge2.y
                );

                tangent.z = f * (
                        deltaUV2.y * edge1.z -
                        deltaUV1.y * edge2.z
                );

                tangents[i0] += tangent;
                tangents[i1] += tangent;
                tangents[i2] += tangent;
        }

        // Normalize and orthogonalize the tangent against the normal
        for (size_t i = 0; i < vertices.size(); ++i) {
                glm::vec3 tangent = tangents[i];

                // Gram-Schmidt orthogonalization
                tangent -= vertices[i].normal * glm::dot(vertices[i].normal, tangent);

                if (glm::dot(tangent, tangent) > 1e-6f) {
                        tangent = glm::normalize(tangent);
                } else {
                        tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                }

                vertices[i].tangent = glm::vec4(tangent, 1.0f);
        }
}
