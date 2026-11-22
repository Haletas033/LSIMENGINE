#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <vector>

#include "meshPool.h"
#include "primitive.h"
#include "transform.h"
#include "../scene/camera.h"
#include "ECS/entityManager.h"
#include "ECS/registry.h"
#include "rendering/material.h"

namespace Mesh {
        inline Device* device = nullptr;
        inline Command* command = nullptr;

        inline void setDevice(Device* d) { device = d; }
        inline void setCommand(Command* c) { command = c; }

        std::expected<EntityHandle, LSIM::Error> create(Primitive::Type primitive, MeshMode mode,
                                                        Registry& registry = Registry::getDefaultRegistry(),
                                                        MeshPool& meshPool = MeshPool::getDefaultMeshPool(), const
                                                        Material& material = Material::createStandardPBR(),
                                                        const Transform& transform = Transform());

        std::expected<EntityHandle, LSIM::Error> create(const std::vector<Vertex>& vertices,
                                                        const std::vector<uint32_t>& indices, MeshMode mode,
                                                        Registry& registry = Registry::getDefaultRegistry(),
                                                        MeshPool& meshPool = MeshPool::getDefaultMeshPool(), const
                                                        Material& material = Material::createStandardPBR(),
                                                        const Transform& transform = Transform());
}
#endif //MESH_CLASS_H
