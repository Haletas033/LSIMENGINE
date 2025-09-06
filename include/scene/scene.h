//
// Created by halet on 9/4/2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <memory>

#include "../geometry/mesh.h"
#include "light.h"

struct Scene {
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<Light> lights;

    // Signals for meshes
    mutable bool addMeshSignal = false;
    mutable bool deleteMeshSignal = false;

    // Signals for lights
    mutable bool addLightSignal = false;
    mutable bool deleteLightSignal = false;

    // Move constructor
    Scene(std::vector<std::unique_ptr<Mesh>>&& m, std::vector<Light>&& l)
        : meshes(std::move(m)), lights(std::move(l)) {}

    // Move assignment
    Scene& operator=(Scene&& other) noexcept {
        meshes = std::move(other.meshes);
        lights = std::move(other.lights);
        addMeshSignal = other.addMeshSignal;
        deleteMeshSignal = other.deleteMeshSignal;
        addLightSignal = other.addLightSignal;
        deleteLightSignal = other.deleteLightSignal;
        return *this;
    }

    // Delete copy operations
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
};

#endif //SCENE_H
