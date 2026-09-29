#ifndef LSIM_RENDERSYSTEM_H
#define LSIM_RENDERSYSTEM_H
#include "ECS/system.h"
#include <geometry/meshPool.h>

#include "light.h"
#include "scene/camera.h"

constexpr GLuint LIGHT_UBO_BINDING_POINT = 1;

extern Defaults engineDefaults;

class RenderSystem final : public System {
private:
        MeshPool& meshPool;
        Camera& camera;
        GLuint lightUBOId;
public:
        RenderSystem(MeshPool& meshPool, Camera& camera) : meshPool(meshPool), camera(camera) {
                glGenBuffers(1, &lightUBOId);
                glBindBuffer(GL_UNIFORM_BUFFER, lightUBOId);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(LightGPUData) * engineDefaults.MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);

                glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_UBO_BINDING_POINT, lightUBOId);

                glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        ~RenderSystem() override {
                glDeleteBuffers(1, &lightUBOId);
        }

        void update(Registry &registry, float deltaTime) override;
};

#endif //LSIM_RENDERSYSTEM_H
