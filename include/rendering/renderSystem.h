#ifndef LSIM_RENDERSYSTEM_H
#define LSIM_RENDERSYSTEM_H
#include "ECS/system.h"
#include <geometry/meshPool.h>

#include "scene/camera.h"

class RenderSystem final : public System {
private:
        MeshPool& meshPool;
        Camera& camera;
public:
        RenderSystem(MeshPool& meshPool, Camera& camera) : meshPool(meshPool), camera(camera){}
        void update(Registry &registry, float deltaTime) override;
};

#endif //LSIM_RENDERSYSTEM_H
