//
// Created by halet on 9/4/2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <optional>
#include <vector>

#include "camera.h"
#include "LSIMtypes.h"
#include "ECS/entityManager.h"
#include "glm/glm.hpp"

class SharedState;

struct Scene {
    glm::vec4 ambientLightColour = glm::vec4(1.0);
    float ambientLightIntensity = 0.1;

    // Signals for lights
    mutable bool addLightSignal = false;
    mutable bool deleteLightSignal = false;

    Scene() = default;

    // Delete copy operations
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    [[nodiscard]] static std::expected<void, LSIM::Error> createDefaultScene(
        SharedState& sharedState, EntityHandle& skybox);
    [[nodiscard]] static std::expected<void, LSIM::Error> initializeScene(
        const Device& device, Command& command, SharedState& sharedState, Camera& camera, EntityHandle& skybox);
};

#endif //SCENE_H
