#include <geometry/mesh.h>

#include "engineContext.h"
#include "ECS/name.h"
#include "geometry/model.h"
#include "geometry/terrain.h"
#include "inputs/gui.h"
#include "rendering/meshRenderer.h"

#include "ECS/name.traits.h"
#include "rendering/material.traits.h"
#include "rendering/meshRenderer.traits.h"
#include "geometry/transform.traits.h"

std::expected<EntityHandle, LSIM::Error> Mesh::create(
        const Primitive::Type primitive,
        const MeshMode mode,
        Registry& registry,
        MeshPool& meshPool,
        const Material& material,
        const Transform& transform
) {
        MeshData meshData{};

        switch (primitive) {
                case Primitive::CUBE:
                        meshData = Primitive::GenerateCube(1.f);
                        break;
                case Primitive::PYRAMID:
                        meshData = Primitive::GeneratePyramid(1.f);
                        break;
                case Primitive::PLANE:
                        meshData = Primitive::GeneratePlane(1.f);
                        break;
                case Primitive::SPHERE:
                        meshData = Primitive::GenerateSphere(24, 24, 1.f);
                        break;
                case Primitive::TORUS:
                        meshData = Primitive::GenerateTorus(24, 12, 1.f, 0.2f,1.f);
                        break;
                case Primitive::TERRAIN: {
                        Defaults defaults = EngineContext::getDefaults();
                        std::vector<std::vector<float>> noiseMap = Terrain::GenerateNoiseMap(defaults.size, defaults.size, static_cast<int>(time(nullptr)),
                                defaults.scale, defaults.octaves, defaults.persistence, defaults.lacunarity);

                        const auto uID = static_cast<long long>(glfwGetTime() * 1'000'000'000LL);
                        const std::string outputPath = std::string(EngineContext::getWorkingDir()) + "resources/" + std::to_string(uID) + "terrain.png";
                        const auto noiseMapTexture = Terrain::noiseMapToTexture(
                                noiseMap,
                                std::to_string(uID),
                                outputPath.c_str()
                        );

                        if (!noiseMapTexture) return std::unexpected(noiseMapTexture.error());

                        std::vector<Vertex> vertices;
                        std::vector<uint32_t> indices;

                        Terrain::noiseMapToMesh(noiseMap, vertices, indices, defaults.heightScale, defaults.gridScale);

                        const auto mesh = create(vertices, indices, mode, registry, meshPool, material, transform);
                        if (!mesh) return std::unexpected(mesh.error());
                        registry.getComponent<Name>(*mesh)->value = "Terrain";
                        Material *mat = registry.getComponent<Material>(*mesh);
                        auto tex = mat->addTexture(
                                "albedo",
                                outputPath,
                                VK_FILTER_NEAREST,
                                TextureType::COLOR
                        );
                        if (!tex) return std::unexpected(tex.error());

                        LSIM_CHECK(mat->setProperty("useTexture", 1));
                        return mesh;
                }
                case Primitive::MODEL: {
                        const auto filePath = IO::OpenDialog("Model Files\0*.gltf\0All Files\0*.*\0");
                        engineLogger("stdInfo", filePath);

                        auto model = Model::create(registry, meshPool, filePath.c_str());
                        if (!model) return std::unexpected(model.error());
                        return model->meshes[0];
                }
        }
        auto mesh = create(meshData.vertices, meshData.indices, mode, registry, meshPool, material, transform);
        if (!mesh) return std::unexpected(mesh.error());
        return *mesh;
}

std::expected<EntityHandle, LSIM::Error> Mesh::create(
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        const MeshMode mode,
        Registry& registry,
        MeshPool& meshPool,
        const Material& material,
        const Transform& transform
) {
        const EntityHandle e = registry.create();
        MeshData data{
                .vertices = vertices,
                .indices = indices
        };
        const auto handle = meshPool.upload(*device, *command, data, mode);
        if (!handle) return std::unexpected(handle.error());
        registry.addComponent<Name>(e, {"Mesh"});
        registry.addComponent<Transform>(e, transform);
        registry.addComponent<Material>(e, material);
        registry.addComponent<MeshRenderer>(e, MeshRenderer{*handle, vertices, indices, mode});

        return e;
}
