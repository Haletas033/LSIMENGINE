#include <rendering/material.traits.h>

#include "glm/gtc/type_ptr.hpp"

void ComponentTraits<Material>::inspect(Registry& registry, SharedState sharedState, const EntityHandle self) {
        if (ImGui::CollapsingHeader("Material")) {
                auto material = registry.getComponent<Material>(self);
                for (const auto &property : material->getProperties()) {
                        std::visit([property, material]<typename T0>(T0 &&arg) {
                                using T = std::remove_cvref_t<T0>;
                                if constexpr (std::is_same_v<int, T>) {
                                        addPropertyField(arg, material, property.first, [](const char* l, T* v) {
                                                return ImGui::InputInt(l, v);
                                        });
                                } else if constexpr (std::is_same_v<float, T>) {
                                        addPropertyField(arg, material, property.first, [](const char* l, T* v) {
                                                return ImGui::InputFloat(l, v);
                                        });
                                } else if constexpr (std::is_same_v<glm::vec3, T>) {
                                        addPropertyField(arg, material, property.first, [](const char* l, T* v) {
                                                   return ImGui::InputFloat3(l, glm::value_ptr(*v));
                                           });
                                } else if constexpr (std::is_same_v<glm::vec4, T>) {
                                        addPropertyField(arg, material, property.first, [](const char* l, T* v) {
                                                return ImGui::ColorEdit4(l, glm::value_ptr(*v));
                                        });
                                } else if constexpr (std::is_same_v<glm::mat3, T>) {
                                        glm::vec3 c0 = arg[0];
                                        glm::vec3 c1 = arg[1];
                                        glm::vec3 c2 = arg[2];

                                        ImGui::TextUnformatted(property.first.c_str());
                                        ImGui::PushID(property.first.c_str());

                                        bool changed = false;
                                        changed |= ImGui::InputFloat3("##X", glm::value_ptr(c0));
                                        changed |= ImGui::InputFloat3("##Y", glm::value_ptr(c1));
                                        changed |= ImGui::InputFloat3("##Z", glm::value_ptr(c2));

                                        ImGui::PopID();

                                        if (changed) {
                                            material->setProperty(property.first, glm::mat3(c0, c1, c2));
                                        }
                                } else if constexpr (std::is_same_v<glm::mat4, T>) {
                                        glm::vec4 c0 = arg[0];
                                        glm::vec4 c1 = arg[1];
                                        glm::vec4 c2 = arg[2];
                                        glm::vec4 c3 = arg[3];

                                        ImGui::TextUnformatted(property.first.c_str());
                                        ImGui::PushID(property.first.c_str());

                                        bool changed = false;
                                        changed |= ImGui::InputFloat4("##X", glm::value_ptr(c0));
                                        changed |= ImGui::InputFloat4("##Y", glm::value_ptr(c1));
                                        changed |= ImGui::InputFloat4("##Z", glm::value_ptr(c2));
                                        changed |= ImGui::InputFloat4("##W", glm::value_ptr(c3));

                                        ImGui::PopID();

                                        if (changed) {
                                            material->setProperty(property.first, glm::mat4(c0, c1, c2, c3));
                                        }
                                }
                        }, property.second);
                }

        }
}
