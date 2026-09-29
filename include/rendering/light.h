#ifndef LSIM_LIGHT_H
#define LSIM_LIGHT_H

#include <glm/glm.hpp>

#include "ECS/registry.h"

class Light {
public:
        enum class Type {
                POINT,
                DIRECTIONAL,
                SPOT
        };

        glm::vec4 lightColor = glm::vec4(1.0f);

        float attenuationScale = 1.0f;
        float intensity = 1.0f;
        float spotAngle = 0.5;

        Type type = Type::POINT;

        static EntityHandle create(Registry &registry, Type lightType);
};

struct LightGPUData {
        glm::vec4 lightColor;
        glm::vec4 position;
        glm::vec4 direction;
        glm::vec4 params; // AttenuationScale, intensity, spotAngle, type;
};

#endif //LSIM_LIGHT_H
