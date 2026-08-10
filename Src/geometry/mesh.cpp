#include <geometry/mesh.h>

#include "ECS/name.h"
#include "rendering/meshRenderer.h"

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
                case Primitive::TERRAIN:
                        break; // TODO
                case Primitive::MODEL:
                        break; // TODO
        }
        return create(meshData.vertices, meshData.indices, mode, registry, meshPool, material, transform);
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
