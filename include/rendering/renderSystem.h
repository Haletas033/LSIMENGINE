#ifndef LSIM_RENDERSYSTEM_H
#define LSIM_RENDERSYSTEM_H
#include "ECS/system.h"
#include <geometry/meshPool.h>

#include "light.h"
#include "scene/camera.h"

class Material;
constexpr GLuint LIGHT_UBO_BINDING_POINT = 1;

extern Defaults engineDefaults;

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
        GLuint lightUBOId;
        GLuint instanceVBO;
public:
        RenderSystem(MeshPool& meshPool, Camera& camera) : meshPool(meshPool), camera(camera) {
                // Lights
                glGenBuffers(1, &lightUBOId);
                glBindBuffer(GL_UNIFORM_BUFFER, lightUBOId);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(LightGPUData) * engineDefaults.MAX_LIGHTS, nullptr,
                             GL_DYNAMIC_DRAW);

                glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_UBO_BINDING_POINT, lightUBOId);

                glBindBuffer(GL_UNIFORM_BUFFER, 0);

                // Instances
                glGenBuffers(1, &instanceVBO);

                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

                glBufferData(
                    GL_ARRAY_BUFFER,
                    sizeof(InstanceGPUData) * 1024,
                    nullptr,
                    GL_DYNAMIC_DRAW
                );

                glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        ~RenderSystem() override {
                glDeleteBuffers(1, &lightUBOId);
                glDeleteBuffers(1, &instanceVBO);
        }

        void update(Registry &registry, float deltaTime) override;
};

#endif //LSIM_RENDERSYSTEM_H
