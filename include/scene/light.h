//
// Created by halet on 9/3/2025.
//

#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
public:
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(0.0f);
    float attenuationScale = 1.0f;
    float invScale = 1.0f / (attenuationScale + 0.001f); // Avoid division by zero
    float linear     = 0.09f * invScale;
    float quadratic  = 0.032f * invScale;
};

#endif //LIGHT_H
