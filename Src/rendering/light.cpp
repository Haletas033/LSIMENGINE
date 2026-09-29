#include <rendering/light.h>

#include "ECS/entityManager.h"
#include "ECS/name.h"
#include "ECS/name.traits.h"
#include "geometry/transform.traits.h"
#include "rendering/light.traits.h"
#include "geometry/transform.h"

EntityHandle Light::create(Registry &registry, const Type lightType) {
        const EntityHandle light = registry.create();
        registry.addComponent<Name>(light, { "light" });
        registry.addComponent<Transform>(light, {});
        registry.addComponent<Light>(light, {
                .lightColor = glm::vec4{1.f},
                .attenuationScale = 1.f,
                .intensity = 1.f,
                .spotAngle = 60.f,
                .type = lightType
        });

        return light;
}
