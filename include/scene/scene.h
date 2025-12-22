//
// Created by halet on 9/4/2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <memory>

#include "../geometry/mesh.h"
#include "light.h"

struct Scene {

    struct InstancedMesh {
        std::unique_ptr<Mesh> mesh;
        std::vector<glm::mat4> instances;
        GLuint instanceVBO = 0;

        void UploadInstances() {
            if (instanceVBO == 0)
                glGenBuffers(1, &instanceVBO);

            glBindVertexArray(mesh->vao.ID);
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(glm::mat4), instances.data(), GL_STATIC_DRAW);

            for (int i = 0; i < 4; ++i) {
                glEnableVertexAttribArray(4 + i);
                glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                    reinterpret_cast<void *>(i * sizeof(glm::vec4)));
                glVertexAttribDivisor(4 + i, 1);
            }
            glBindVertexArray(0);
        }

        void DrawInstances(const Shader &shader, const Camera &camera) {
            glBindVertexArray(mesh->vao.ID);
            glDrawElementsInstanced(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr, instances.size());
        }
    };


    std::vector<std::vector<std::unique_ptr<Mesh>>> meshes;
    std::vector<std::unique_ptr<InstancedMesh>> instancedMeshes;
    std::vector<Light> lights;

    glm::vec4 ambientLightColour = glm::vec4(1.0);
    float ambientLightIntensity = 0.1;

    // Signals for meshes
    mutable bool addMeshSignal = false;
    mutable bool deleteMeshSignal = false;

    // Signals for lights
    mutable bool addLightSignal = false;
    mutable bool deleteLightSignal = false;

    Scene() = default;

    // Move constructor
    Scene(std::vector<std::vector<std::unique_ptr<Mesh>>>&& m, std::vector<Light>&& l)
    : meshes(std::move(m)), lights(std::move(l)) {}

    // Move assignment
    Scene& operator=(Scene&& other) noexcept {
        meshes = std::move(other.meshes);
        lights = std::move(other.lights);
        return *this;
    }

    // Delete copy operations
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
};

#endif //SCENE_H
