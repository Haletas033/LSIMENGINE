#ifndef LSIM_TRANSFORMSYSTEM_H
#define LSIM_TRANSFORMSYSTEM_H
#include "transform.h"
#include "ECS/system.h"

class TransformSystem final : public System {
public:
        void update(Registry &registry, float deltaTime) override {
                for (const EntityHandle& e : registry.getAllAlive()) {
                        if (!registry.hasComponent<Transform>(e)) continue;
                        auto* transform = registry.getComponent<Transform>(e);
                        transform->recomputeIfDirty();
                }
        }
};

#endif //LSIM_TRANSFORMSYSTEM_H
