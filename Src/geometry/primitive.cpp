#include "geometry/primitive.h"

constexpr float PI = 3.14159265359f;

MeshData Primitive::GeneratePlane(const float tileScale) {
        MeshData mesh{
                .vertices{
                        {.position = {-0.5f, 0.0f, -0.5f,}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {0.f, 0.f}},
                        {.position = {0.5f, 0.0f, -0.5f}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {tileScale, 0.f}},
                        {.position = {0.5f, 0.0f, 0.5f}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {tileScale, tileScale}},
                        {.position = {-0.5f, 0.0f, 0.5f}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {0.f, tileScale}},
                },
                .indices{
                        0, 1, 2,
                        2, 3, 0
                }
        };

        mesh.GenerateTangents();
        return mesh;
}

MeshData Primitive::GenerateCube(const float tileScale) {
        MeshData mesh{
                .vertices{
                        // Front
                        {.position = {-0.5f, -0.5f, 0.5f}, .normal = {0.f, 0.f, 1.f}, .tangent = {}, .uv = {0.f, 0.f}},
                        {.position = {0.5f, -0.5f, 0.5f}, .normal = {0.f, 0.f, 1.f}, .tangent = {}, .uv = {tileScale, 0.f}},
                        {.position = {0.5f, 0.5f, 0.5f}, .normal = {0.f, 0.f, 1.f}, .tangent = {}, .uv = {tileScale, tileScale}},
                        {.position = {-0.5f, 0.5f, 0.5f}, .normal = {0.f, 0.f, 1.f}, .tangent = {}, .uv = {0.f, tileScale}},

                        // Back
                        {.position = {0.5f, -0.5f, -0.5f}, .normal = {0.f, 0.f, -1.f}, .tangent = {}, .uv = {0.f, 0.f}},
                        {.position = {-0.5f, -0.5f, -0.5f}, .normal = {0.f, 0.f, -1.f}, .tangent = {}, .uv = {tileScale, 0.f}},
                        {.position = {-0.5f, 0.5f, -0.5f}, .normal = {0.f, 0.f, -1.f}, .tangent = {}, .uv = {tileScale, tileScale}},
                        {.position = {0.5f, 0.5f, -0.5f}, .normal = {0.f, 0.f, -1.f}, .tangent = {}, .uv = {0.f, tileScale}},

                        // Left
                        {.position = {-0.5f, -0.5f, -0.5f}, .normal = {-1.f, 0.f, 0.f}, .tangent = {}, .uv = {0.f, 0.f}},
                        {.position = {-0.5f, -0.5f, 0.5f}, .normal = {-1.f, 0.f, 0.f}, .tangent = {}, .uv = {tileScale, 0.f}},
                        {.position = {-0.5f, 0.5f, 0.5f}, .normal = {-1.f, 0.f, 0.f}, .tangent = {}, .uv = {tileScale, tileScale}},
                        {.position = {-0.5f, 0.5f, -0.5f}, .normal = {-1.f, 0.f, 0.f}, .tangent = {}, .uv = {0.f, tileScale}},

                        // Right
                        {.position = {0.5f, -0.5f, 0.5f}, .normal = {1.f, 0.f, 0.f}, .tangent = {}, .uv = {0.f, 0.f}},
                        {.position = {0.5f, -0.5f, -0.5f}, .normal = {1.f, 0.f, 0.f}, .tangent = {}, .uv = {tileScale, 0.f}},
                        {.position = {0.5f, 0.5f, -0.5f}, .normal = {1.f, 0.f, 0.f}, .tangent = {}, .uv = {tileScale, tileScale}},
                        {.position = {0.5f, 0.5f, 0.5f}, .normal = {1.f, 0.f, 0.f}, .tangent = {}, .uv = {0.f, tileScale}},

                        // Top
                        {.position = {-0.5f, 0.5f, 0.5f}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {0.f, 0.f}},
                        {.position = {0.5f, 0.5f, 0.5f}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {tileScale, 0.f}},
                        {.position = {0.5f, 0.5f, -0.5f}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {tileScale, tileScale}},
                        {.position = {-0.5f, 0.5f, -0.5f}, .normal = {0.f, 1.f, 0.f}, .tangent = {}, .uv = {0.f, tileScale}},

                        // Bottom
                        {.position = {-0.5f, -0.5f, -0.5f}, .normal = {0.f, -1.f, 0.f}, .tangent = {}, .uv = {0.f, 0.f}},
                        {.position = {0.5f, -0.5f, -0.5f}, .normal = {0.f, -1.f, 0.f}, .tangent = {}, .uv = {1.f, 0.f}},
                        {.position = {0.5f, -0.5f, 0.5f}, .normal = {0.f, -1.f, 0.f}, .tangent = {}, .uv = {1.f, 1.f}},
                        {.position = {-0.5f, -0.5f, 0.5f}, .normal = {0.f, -1.f, 0.f}, .tangent = {}, .uv = {0.f, 1.f}},
                },
                .indices{
                        // Front
                        0, 1, 2,
                        0, 2, 3,

                        // Back
                        4, 5, 6,
                        4, 6, 7,

                        // Left
                        8, 9, 10,
                        8, 10, 11,

                        // Right
                        12, 13, 14,
                        12, 14, 15,

                        // Top
                        16, 17, 18,
                        16, 18, 19,

                        // Bottom
                        20, 21, 22,
                        20, 22, 23,
                }
        };

        mesh.GenerateTangents();
        return mesh;
}

MeshData Primitive::GeneratePyramid(const float tileScale) {
        MeshData mesh;

        constexpr glm::vec3 v0{-0.5f, 0.0f, 0.5f};
        constexpr glm::vec3 v1{0.5f, 0.0f, 0.5f};
        constexpr glm::vec3 v2{0.5f, 0.0f, -0.5f};
        constexpr glm::vec3 v3{-0.5f, 0.0f, -0.5f};
        constexpr glm::vec3 tip{0.0f, 0.8f, 0.0f};

        auto addFace = [&](const glm::vec3 &a,
                           const glm::vec3 &b,
                           const glm::vec3 &c) {
                const glm::vec3 normal =
                                glm::normalize(glm::cross(b - a, c - a));

                const auto base = static_cast<uint32_t>(mesh.vertices.size());

                mesh.vertices.push_back({
                        .position = a,
                        .normal = normal,
                        .tangent = {},
                        .uv = {0.0f, 0.0f}
                });

                mesh.vertices.push_back({
                        .position = b,
                        .normal = normal,
                        .tangent = {},
                        .uv = {tileScale, 0.0f}
                });

                mesh.vertices.push_back({
                        .position = c,
                        .normal = normal,
                        .tangent = {},
                        .uv = {tileScale * 0.5f, tileScale}
                });

                mesh.indices.insert(mesh.indices.end(), {
                        base, base + 1, base + 2
                });
        };

        // Sides
        addFace(v0, v1, tip);
        addFace(v1, v2, tip);
        addFace(v2, v3, tip);
        addFace(v3, v0, tip);

        // Bottom
        const auto base = static_cast<uint32_t>(mesh.vertices.size());

        mesh.vertices.insert(mesh.vertices.end(), {
                {.position = v3, .normal = {0.0f, -1.0f, 0.0f}, .tangent = {}, .uv = {0.0f, 0.0f}},
                {.position = v2, .normal = {0.0f, -1.0f, 0.0f}, .tangent = {}, .uv = {tileScale, 0.0f}},
                {.position = v1, .normal = {0.0f, -1.0f, 0.0f}, .tangent = {}, .uv = {tileScale, tileScale}},
                {.position = v0, .normal = {0.0f, -1.0f, 0.0f}, .tangent = {}, .uv = {0.0f, tileScale}}
        });

        mesh.indices.insert(mesh.indices.end(), {
                base, base + 1, base + 2,
                base, base + 2, base + 3
        });

        mesh.GenerateTangents();
        return mesh;
}

MeshData Primitive::GenerateSphere(const int stacks, const int slices, const float tileScale) {
        MeshData mesh{};

        for (int i = 0; i <= stacks; ++i) {
                const float V = static_cast<float>(i) / static_cast<float>(stacks);
                const float phi = V * PI;

                for (int j = 0; j <= slices; ++j) {
                        const float U = static_cast<float>(j) / static_cast<float>(slices);
                        const float theta = U * 2.0f * PI;

                        float x = cosf(theta) * sinf(phi);
                        float y = cosf(phi);
                        float z = sinf(theta) * sinf(phi);

                        mesh.vertices.push_back({
                                .position = {
                                        x * 0.5f,
                                        y * 0.5f,
                                        z * 0.5f
                                },
                                .normal = {x, y, z},
                                .tangent = {},
                                .uv = {
                                        U * tileScale,
                                        V * tileScale
                                }
                        });
                }
        }

        // Indices
        for (int i = 0; i < stacks; ++i) {
                for (int j = 0; j < slices; ++j) {
                        const int first = i * (slices + 1) + j;
                        const int second = first + slices + 1;

                        mesh.indices.push_back(first);
                        mesh.indices.push_back(first + 1);
                        mesh.indices.push_back(second);

                        mesh.indices.push_back(second);
                        mesh.indices.push_back(first + 1);
                        mesh.indices.push_back(second + 1);
                }
        }

        mesh.GenerateTangents();
        return mesh;
}

MeshData Primitive::GenerateTorus(const int ringSegments, const int tubeSegments,
                                  const float ringRadius, const float tubeRadius,
                                  const float tileScale
) {
        MeshData mesh{};

        for (int i = 0; i <= ringSegments; ++i) {
                const float u = static_cast<float>(i) / static_cast<float>(ringSegments) * 2.0f * PI;
                const float cosU = cosf(u);
                const float sinU = sinf(u);

                for (int j = 0; j <= tubeSegments; ++j) {
                        const float v = static_cast<float>(j) / static_cast<float>(tubeSegments) * 2.0f * PI;
                        const float cosV = cosf(v);
                        const float sinV = sinf(v);

                        float x = (ringRadius + tubeRadius * cosV) * cosU;
                        float y = tubeRadius * sinV;
                        float z = (ringRadius + tubeRadius * cosV) * sinU;

                        // Normals
                        float nx = cosU * cosV;
                        float ny = sinV;
                        float nz = sinU * cosV;

                        mesh.vertices.push_back({
                                .position = {x, y, z},
                                .normal = {nx, ny, nz},
                                .tangent = {},
                                .uv = {
                                        static_cast<float>(i) / static_cast<float>(ringSegments) * tileScale,
                                        static_cast<float>(j) / static_cast<float>(tubeSegments) * tileScale
                                }
                        });
                }
        }

        for (int i = 0; i < ringSegments; ++i) {
                for (int j = 0; j < tubeSegments; ++j) {
                        const int first = i * (tubeSegments + 1) + j;
                        const int second = first + tubeSegments + 1;

                        mesh.indices.push_back(first);
                        mesh.indices.push_back(first + 1);
                        mesh.indices.push_back(second);

                        mesh.indices.push_back(second);
                        mesh.indices.push_back(first + 1);
                        mesh.indices.push_back(second + 1);
                }
        }

        mesh.GenerateTangents();
        return mesh;
}
