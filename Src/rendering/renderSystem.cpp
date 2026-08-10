#include <rendering/renderSystem.h>

#include "geometry/transform.h"
#include "rendering/material.h"
#include "rendering/meshRenderer.h"
#include "resources/resourceManager.h"


void RenderSystem::update(Registry &registry, float deltaTime) {
        for (const EntityHandle& e : registry.getAllAlive()) {
                if (!registry.hasComponent<MeshRenderer>(e)) continue;
                if (!registry.hasComponent<Transform>(e)) continue;
                if (!registry.hasComponent<Material>(e)) continue;

                const auto* transform = registry.getComponent<Transform>(e);
                const auto* meshRenderer = registry.getComponent<MeshRenderer>(e);
                const auto* material = registry.getComponent<Material>(e);

                glm::mat4 model = transform->getModelMatrix();

                const Shader *shader = ResourceManager::getShaderById(material->getShader());
                shader->Activate();

                GLuint modelLoc = shader->GetLocation("model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                GLuint normalLoc = shader->GetLocation("normalMatrix");
                glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
                glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
                GLuint viewPosLoc = shader->GetLocation("viewPos");
                glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));

                int unit = 0;
                for (const auto& [name, texId] : material->getTextures()) {
                        glActiveTexture(GL_TEXTURE0 + unit);
                        glBindTexture(GL_TEXTURE_2D, texId);
                        shader->SetInt(name, unit);
                        ++unit;
                }

                for (const auto& [name, property] : material->getProperties()) {
                        std::visit([&]<typename T0>(T0&& value) {
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

                meshPool.get(meshRenderer->meshHandle)->vao.Bind();

                glDrawElements(GL_TRIANGLES, meshPool.get(meshRenderer->meshHandle)->indexCount, GL_UNSIGNED_INT, nullptr);
        }
}
