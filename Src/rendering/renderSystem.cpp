#include <rendering/renderSystem.h>

#include "geometry/transform.h"
#include "rendering/material.h"
#include "rendering/meshRenderer.h"
#include "resources/resourceManager.h"

void RenderSystem::update(Registry &registry, float deltaTime) {
        std::unordered_map<BatchKey, RenderBatch, BatchKeyHash> batches;

        for (const EntityHandle &e: registry.getAllAlive()) {
                if (!registry.hasComponent<MeshRenderer>(e)) continue;
                if (!registry.hasComponent<Transform>(e)) continue;
                if (!registry.hasComponent<Material>(e)) continue;

                const auto *transform = registry.getComponent<Transform>(e);
                const auto *meshRenderer = registry.getComponent<MeshRenderer>(e);
                auto *material = registry.getComponent<Material>(e);

                BatchKey key{
                        meshRenderer->meshHandle,
                        material
                };

                auto &[instances] = batches[key];

                InstanceGPUData instance{};
                instance.model = transform->getModelMatrix();
                instance.normalMatrix =
                                glm::transpose(
                                        glm::inverse(
                                                glm::mat3(instance.model)
                                        )
                                );

                instances.push_back(instance);
        }

        std::vector<LightGPUData> lightDatas;
        lightDatas.reserve(engineDefaults.MAX_LIGHTS);

        for (const EntityHandle &e: registry.getAllAlive()) {
                if (!registry.hasComponent<Light>(e)) continue;
                if (!registry.hasComponent<Transform>(e)) continue;

                const auto *transform = registry.getComponent<Transform>(e);
                const auto *light = registry.getComponent<Light>(e);

                LightGPUData lightData{
                        .lightColor = light->lightColor,
                        .position = glm::vec4(transform->getPosition(), 1),
                        .direction = glm::vec4(transform->getRotation() * glm::vec3(0, 0, -1), 0),
                        .params = glm::vec4(light->attenuationScale, light->intensity, glm::radians(light->spotAngle),
                                            static_cast<float>(static_cast<int>(light->type)))
                };

                if (lightDatas.size() < engineDefaults.MAX_LIGHTS)
                        lightDatas.push_back(lightData);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, lightUBOId);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, lightDatas.size() * sizeof(LightGPUData), lightDatas.data());

        for (auto &[key, batch]: batches) {
                Material *material = key.material;
                GPUMeshBuffer *mesh = meshPool.get(key.mesh);

                const Shader *shader = ResourceManager::getShader(material->getShader());

                shader->Activate();

                const GLuint shaderId = shader->GetID();

                glUniformBlockBinding(
                        shaderId,
                        glGetUniformBlockIndex(shaderId, "lightData"),
                        LIGHT_UBO_BINDING_POINT
                );

                shader->SetInt("lightCount", static_cast<int>(lightDatas.size()));
                shader->SetVec3(
                        "viewPos",
                        1,
                        glm::value_ptr(camera.Position)
                );

                // Material textures
                int unit = 0;

                // Material properties
                for (const auto &[name, property]: material->getProperties()) {
                        std::visit([&]<typename T0>(T0 &&value) {
                                using T = std::decay_t<T0>;

                                if constexpr (std::is_same_v<T, float>) {
                                        shader->SetFloat(name, value);
                                } else if constexpr (std::is_same_v<T, int>) {
                                        shader->SetInt(name, value);
                                } else if constexpr (std::is_same_v<T, glm::vec3>) {
                                        shader->SetVec3(name, 1, glm::value_ptr(value));
                                } else if constexpr (std::is_same_v<T, glm::vec4>) {
                                        shader->SetVec4(name, 1, glm::value_ptr(value));
                                } else if constexpr (std::is_same_v<T, glm::mat3>) {
                                        shader->SetMat3(name, 1, glm::value_ptr(value));
                                } else if constexpr (std::is_same_v<T, glm::mat4>) {
                                        shader->SetMat4(name, 1, glm::value_ptr(value));
                                }
                        }, property);
                }

                // Textures
                shader->SetInt("useTexture", 0);
                shader->SetInt("useNormalMap", 0);
                shader->SetInt("useSpecular", 0);
                shader->SetInt("useEmissive", 0);
                for (const auto &name: material->getTextures() | std::views::keys) {
                        GLuint texture = ResourceManager::getTexture(name);
                        if (name == "albedo") {
                                glActiveTexture(GL_TEXTURE0 + unit);
                                glBindTexture(GL_TEXTURE_2D, texture);
                                shader->SetInt("albedo", unit);
                                shader->SetInt("useTexture", 1);
                        } else if (name == "normal") {
                                glActiveTexture(GL_TEXTURE0 + unit);
                                glBindTexture(GL_TEXTURE_2D, texture);
                                shader->SetInt("normal", unit);
                                shader->SetInt("useNormalMap", 1);
                        } else if (name == "specular") {
                                glActiveTexture(GL_TEXTURE0 + unit);
                                glBindTexture(GL_TEXTURE_2D, texture);
                                shader->SetInt("specular", unit);
                                shader->SetInt("useSpecular", 1);
                        } else if (name == "emissive") {
                                glActiveTexture(GL_TEXTURE0 + unit);
                                glBindTexture(GL_TEXTURE_2D, texture);
                                shader->SetInt("emissive", unit);
                                shader->SetInt("useEmissive", 1);
                        } else {
                                glActiveTexture(GL_TEXTURE0 + unit);
                                glBindTexture(GL_TEXTURE_2D, texture);
                                shader->SetInt(name, unit);
                        }
                        ++unit;
                }

                // Upload instance data
                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

                glBufferSubData(
                        GL_ARRAY_BUFFER,
                        0,
                        batch.instances.size() * sizeof(InstanceGPUData),
                        batch.instances.data()
                );

                mesh->configureInstanceAttributes(instanceVBO);
                mesh->vao.Bind();


                glDrawElementsInstanced(
                        GL_TRIANGLES,
                        mesh->indexCount,
                        GL_UNSIGNED_INT,
                        nullptr,
                        static_cast<GLsizei>(batch.instances.size())
                );
        }
}
