#include <geometry/transform.traits.h>
#include "imgui.h"
#include "ECS/entityManager.h"
#include "ECS/registry.h"
#include "editor/sharedState.h"
#include "glm/gtc/type_ptr.hpp"

void ComponentTraits<Transform>::inspect(Registry& registry, SharedState sharedState, const EntityHandle self) {
        if (ImGui::CollapsingHeader("Transform")) {
                const auto transform = registry.getComponent<Transform>(self);

                static bool uniformScaleLock = true;
                float uniformScale = transform->getScale().x;

                glm::vec3 position = transform->getPosition();
                glm::vec3 rotation = transform->getRotationEuler();
                glm::vec3 scale = transform->getScale();

                const auto currentMeshes = sharedState.current_meshes();

                if (ImGui::InputFloat3("Position", glm::value_ptr(position))) {
                        for (const EntityHandle e : currentMeshes) {
                                if (auto* t = registry.getComponent<::Transform>(e)) {
                                        t->setPosition(position);
                                }
                        }
                }
                if (ImGui::InputFloat3("Rotation", glm::value_ptr(rotation))) {
                        for (const EntityHandle e : currentMeshes) {
                                if (auto* t = registry.getComponent<::Transform>(e)) {
                                        t->setRotation(rotation);
                                }
                        }
                }

                if (uniformScaleLock) {
                        if (ImGui::InputFloat("Scale", &uniformScale, 0.1f)) {
                                for (const EntityHandle e : currentMeshes) {
                                        if (auto* t = registry.getComponent<::Transform>(e)) {
                                                t->setScale(glm::vec3(uniformScale));
                                        }
                                }
                        }
                } else {
                        if (ImGui::InputFloat3("Scale", glm::value_ptr(scale))) {
                                for (const EntityHandle e : currentMeshes) {
                                        if (auto* t = registry.getComponent<::Transform>(e)) {
                                                t->setScale(scale);
                                        }
                                }
                        }
                }

                ImGui::Checkbox("Uniform Scale", &uniformScaleLock);
        }
}
