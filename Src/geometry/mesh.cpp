#include <geometry/mesh.h>

#include "ECS/name.h"
#include "geometry/terrain.h"
#include "inputs/gui.h"
#include "rendering/meshRenderer.h"

extern Defaults engineDefaults;
extern std::string workingDir;

EntityHandle Mesh::create(const Primitive::Type primitive, const MeshMode mode, Registry &registry, MeshPool &meshPool, const Material &material, const Transform &transform) {
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
                        std::vector<std::vector<float>> noiseMap = Terrain::GenerateNoiseMap(engineDefaults.size, engineDefaults.size, static_cast<int>(time(nullptr)),
                                engineDefaults.scale, engineDefaults.octaves, engineDefaults.persistence, engineDefaults.lacunarity);

                        const auto uID = static_cast<long long>(glfwGetTime() * 1'000'000'000LL);
                        const std::string outputPath = std::string(workingDir) + "resources/" + std::to_string(uID) + "terrain.png";
                        const uint32_t noiseMapTexture = Terrain::noiseMapToTexture(noiseMap, outputPath.c_str());

                        std::vector<float> vertices;
                        std::vector<uint32_t> indices;

                        Terrain::noiseMapToMesh(noiseMap, vertices, indices, engineDefaults.heightScale, engineDefaults.gridScale);

                        const EntityHandle mesh = create(vertices, indices, mode, registry, meshPool, material, transform);
                        registry.getComponent<Name>(mesh)->value = "Terrain";
                        Material *mat = registry.getComponent<Material>(mesh);
                        mat->setProperty("useTexture", 1);
                        // newMesh->texturePath = std::to_string(uID) + "terrain.png";
                        mat->addProperty<int>("texId", static_cast<int>(noiseMapTexture));
                        auto* node = new Gui::Node{ mesh, Gui::root, {} };
                        Gui::root->children.push_back(node);
                        return mesh;
                }
                case Primitive::MODEL:
                        break; // TODO
        }
        EntityHandle mesh = create(meshData.vertices, meshData.indices, mode, registry, meshPool, material, transform);
        auto* node = new Gui::Node{ mesh, Gui::root, {} };
        Gui::root->children.push_back(node);
        return mesh;
}

EntityHandle Mesh::create(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const MeshMode mode,
        Registry &registry, MeshPool &meshPool, const Material &material, const Transform &transform)
{
        const EntityHandle e = registry.create();
        MeshData data{ vertices, {}, indices };
        const MeshHandle handle = meshPool.upload(data, mode);
        registry.addComponent<Name>(e, {"Mesh"});
        registry.addComponent<Material>(e, material);
        registry.addComponent<MeshRenderer>(e, MeshRenderer{handle, mode});
        registry.addComponent<Transform>(e, transform);

        return e;
}
