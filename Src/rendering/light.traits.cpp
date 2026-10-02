#include "rendering/light.traits.h"

#include "glm/gtc/type_ptr.hpp"

void ComponentTraits<Light>::inspect(Registry &registry, SharedState &sharedState, const EntityHandle &self) {
        if (ImGui::CollapsingHeader("Light")) {
                const auto light = registry.getComponent<Light>(self);

                int type = static_cast<int>(light->type);

                const char* items[] = { "Point", "Directional", "Spot" };

                if (ImGui::Combo("Type", &type, items, IM_ARRAYSIZE(items))) {
                        for (const EntityHandle e : sharedState.current_entities()) {
                                if (auto* l = registry.getComponent<Light>(e)) {
                                        l->type = static_cast<Light::Type>(type);
                                }
                        }
                }

                if (ImGui::ColorEdit4("Color", glm::value_ptr(light->lightColor))) {
                        for (const EntityHandle e : sharedState.current_entities()) {
                                if (auto* l = registry.getComponent<Light>(e)) {
                                        l->lightColor = light->lightColor;
                                }
                        }
                }

                if (ImGui::SliderFloat("Intensity", &light->intensity, 0, 100)) {
                        for (const EntityHandle e : sharedState.current_entities()) {
                                if (auto* l = registry.getComponent<Light>(e)) {
                                        l->intensity = light->intensity;
                                }
                        }
                }

                if (ImGui::SliderFloat("Attenuation", &light->attenuationScale, 0, 100)) {
                        for (const EntityHandle e : sharedState.current_entities()) {
                                if (auto* l = registry.getComponent<Light>(e)) {
                                        l->attenuationScale = light->attenuationScale;
                                }
                        }
                }

                if (light->type == Light::Type::SPOT) {
                        if (ImGui::SliderFloat("Spot Angle", &light->spotAngle, 0, 90)) {
                                for (const EntityHandle e : sharedState.current_entities()) {
                                        if (auto* l = registry.getComponent<Light>(e)) {
                                                l->spotAngle = light->spotAngle;
                                        }
                                }
                        }
                }
        }
}

std::vector<uint8_t> ComponentTraits<Light>::serialize(Registry &registry, EntityHandle self) {
        const auto light = registry.getComponent<Light>(self);
        std::vector<uint8_t> data{};

        const auto append = [&data](const void* ptr, const size_t size) {
                const auto* bytes = static_cast<const uint8_t*>(ptr);
                data.insert(data.end(), bytes, bytes + size);
        };

        append(&light->lightColor, sizeof(light->lightColor));
        append(&light->attenuationScale, sizeof(light->attenuationScale));
        append(&light->intensity, sizeof(light->intensity));
        append(&light->spotAngle, sizeof(light->spotAngle));
        append(&light->type, sizeof(light->type));

        return data;
}

std::any ComponentTraits<Light>::deserialize(const std::vector<uint8_t> &data, uint64_t &ptr) {
        Light result{};

        const auto read = [&data, &ptr](void* dst, const size_t size) {
                if (ptr + size > data.size()) {
                        throw std::runtime_error("Invalid Light data");
                }

                std::memcpy(dst, data.data() + ptr, size);
                ptr += size;
        };

        read(&result.lightColor, sizeof(result.lightColor));
        read(&result.attenuationScale, sizeof(result.attenuationScale));
        read(&result.intensity, sizeof(result.intensity));
        read(&result.spotAngle, sizeof(result.spotAngle));
        read(&result.type, sizeof(result.type));

        return result;
}
