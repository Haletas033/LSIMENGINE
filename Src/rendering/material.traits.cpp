#include <rendering/material.traits.h>

#include "glm/gtc/type_ptr.hpp"

void ComponentTraits<Material>::inspect(Registry& registry, SharedState &sharedState, const EntityHandle self) {
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

std::vector<uint8_t> ComponentTraits<Material>::serialize(Registry& registry, const EntityHandle self) {
        const auto material = registry.getComponent<Material>(self);
        std::vector<uint8_t> data{};

        const auto append = [&data](const void* ptr, const size_t size) {
                const auto* bytes = static_cast<const uint8_t*>(ptr);
                data.insert(data.end(), bytes, bytes + size);
        };

        // Property count
        const auto propertyCount =
                static_cast<std::uint32_t>(material->getProperties().size());
        append(&propertyCount, sizeof(propertyCount));

        for (const auto& [name, value] : material->getProperties()) {
                // Serialize name
                const auto nameSize = static_cast<std::uint32_t>(name.size());
                append(&nameSize, sizeof(nameSize));
                append(name.data(), name.size());

                // Serialize variant index
                const auto type = static_cast<PropertyType>(value.index());
                append(&type, sizeof(type));

                std::visit([&append](const auto& v) {
                    append(&v, sizeof(v));
                }, value);
        }

        // Texture count
        const auto textureCount =
                static_cast<std::uint32_t>(material->getTextures().size());
        append(&textureCount, sizeof(textureCount));

        for (const auto& [name, path] : material->getTextures()) {
                // Serialize name
                const auto nameSize = static_cast<std::uint32_t>(name.size());
                append(&nameSize, sizeof(nameSize));
                append(name.data(), name.size());

                // Serialize path
                const auto pathSize = static_cast<std::uint32_t>(path.size());
                append(&pathSize, sizeof(pathSize));
                append(path.data(), path.size());
        }

        const std::string shader = material->getShader();
        const auto shaderSize = static_cast<std::uint32_t>(shader.size());
        append(&shaderSize, sizeof(shaderSize));
        append(shader.data(), shader.size());

        return data;
}

std::any ComponentTraits<Material>::deserialize(const std::vector<uint8_t> &data, size_t &ptr) {
        Material result{};

        const auto read = [&data, &ptr](void* dst, const size_t size) {
                if (ptr + size > data.size()) {
                        throw std::runtime_error("Invalid Transform data");
                }

                std::memcpy(dst, data.data() + ptr, size);
                ptr += size;
        };

        const auto readString = [&read]() -> std::string {
                std::uint32_t size;
                read(&size, sizeof(size));

                std::string str(size, '\0');

                if (size > 0) {
                        read(str.data(), size);
                }

                return str;
        };

        // Properties
        std::uint32_t propertyCount;
        read(&propertyCount, sizeof(propertyCount));

        for (std::uint32_t i = 0; i < propertyCount; ++i) {
                const std::string name = readString();

                PropertyType type;
                read(&type, sizeof(type));

                switch (type) {
                        case PropertyType::INT:
                                int intValue;
                                read(&intValue, sizeof(intValue));
                                result.setProperty(name, intValue);
                                break;
                        case PropertyType::FLOAT:
                                float floatValue;
                                read(&floatValue, sizeof(floatValue));
                                result.setProperty(name, floatValue);
                                break;
                        case PropertyType::VEC3:
                                glm::vec3 vec3;
                                read(&vec3, sizeof(vec3));
                                result.setProperty(name, vec3);
                                break;
                        case PropertyType::VEC4:
                                glm::vec4 vec4;
                                read(&vec4, sizeof(vec4));
                                result.setProperty(name, vec4);
                                break;
                        case PropertyType::MAT3:
                                glm::mat3 mat3;
                                read(&mat3, sizeof(mat3));
                                result.setProperty(name, mat3);
                                break;
                        case PropertyType::MAT4:
                                glm::mat4 mat4;
                                read(&mat4, sizeof(mat4));
                                result.setProperty(name, mat4);
                                break;
                }
        }

        // Textures
        std::uint32_t textureCount;
        read(&textureCount, sizeof(textureCount));

        for (std::uint32_t i = 0; i < textureCount; ++i) {
                const std::string name = readString();
                const std::string path = readString();

                result.setTexture(name, path);
        }

        // Shader
        const std::string shader = readString();

        result.addShader(shader);

        return result;
}
