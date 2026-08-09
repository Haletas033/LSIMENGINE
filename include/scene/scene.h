//
// Created by halet on 9/4/2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include "light.h"
#include <vector>

struct Scene {
    std::vector<Light> lights;

    glm::vec4 ambientLightColour = glm::vec4(1.0);
    float ambientLightIntensity = 0.1;

    // Signals for lights
    mutable bool addLightSignal = false;
    mutable bool deleteLightSignal = false;

    Scene() = default;

    // Move constructor
    explicit Scene(std::vector<Light>&& l)
    : lights(std::move(l)) {}

    // Move assignment
    Scene& operator=(Scene&& other) noexcept {
        lights = std::move(other.lights);
        return *this;
    }

    // Delete copy operations
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
};

#endif //SCENE_H
