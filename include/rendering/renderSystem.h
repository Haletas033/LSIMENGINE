#ifndef LSIM_RENDERSYSTEM_H
#define LSIM_RENDERSYSTEM_H
#include "ECS/system.h"
#include <geometry/meshPool.h>

#include "engineContext.h"
#include "light.h"
#include "scene/camera.h"
#include "scene/scene.h"

class Material;

struct RenderBatch {
        std::vector<InstanceGPUData> instances;
};

struct BatchKey {
        MeshHandle mesh;
        Material* material;

        bool operator==(const BatchKey& other) const noexcept {
                return mesh.getIndex() == other.mesh.getIndex() &&
                       material == other.material;
        }
};

struct BatchKeyHash {
        std::size_t operator()(const BatchKey& key) const noexcept {
                const std::size_t h1 =
                    std::hash<uint32_t>{}(key.mesh.getIndex());

                const std::size_t h2 =
                    std::hash<Material*>{}(key.material);

                return h1 ^ (h2 << 1);
        }
};

class RenderSystem final : public System {
private:
        MeshPool& meshPool;
        Camera& camera;
        Scene& scene;
        float& aspect;

public:
        RenderSystem(MeshPool& meshPool, Camera& camera, Scene& scene, float& aspect) : meshPool(meshPool), camera(camera), scene(scene), aspect(aspect) {}


        void update(Context& context, Registry &registry, float deltaTime) override;
};

#endif //LSIM_RENDERSYSTEM_H
