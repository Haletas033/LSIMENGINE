#include <geometry/transform.traits.h>
#include "imgui.h"
#include "ECS/entityManager.h"
#include "ECS/registry.h"
#include "editor/sharedState.h"
#include "glm/gtc/type_ptr.hpp"

void ComponentTraits<Transform>::inspect(Registry& registry, SharedState &sharedState, const EntityHandle self) {
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

std::vector<uint8_t> ComponentTraits<Transform>::serialize(Registry& registry, const EntityHandle self) {
        const auto transform = registry.getComponent<Transform>(self);
        std::vector<uint8_t> data{};

        const auto append = [&data](const void* ptr, const size_t size) {
                const auto* bytes = static_cast<const uint8_t*>(ptr);
                data.insert(data.end(), bytes, bytes + size);
        };

        constexpr uint32_t idSize = id.size();
        append(&idSize, sizeof(idSize));
        append(id.data(), id.size());

        const auto position = transform->getPosition();
        const auto rotation = transform->getRotation();
        const auto scale = transform->getScale();

        append(&position, sizeof(position));
        append(&rotation, sizeof(rotation));
        append(&scale, sizeof(scale));

        return data;
}

std::any ComponentTraits<Transform>::deserialize(const std::vector<uint8_t> &data, size_t &ptr) {
        Transform result{};

        const auto read = [&data, &ptr](void* dst, const size_t size) {
                if (ptr + size > data.size()) {
                        throw std::runtime_error("Invalid Transform data");
                }

                std::memcpy(dst, data.data() + ptr, size);
                ptr += size;
        };

        auto position = result.getPosition();
        auto rotation = result.getRotation();
        auto scale = result.getScale();

        read(&position, sizeof(position));
        read(&rotation, sizeof(rotation));
        read(&scale, sizeof(scale));

        result.setPosition(position);
        result.setRotation(rotation);
        result.setScale(scale);

        return result;
}
