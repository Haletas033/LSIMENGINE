#ifndef LSIM_MATERIAL_H
#define LSIM_MATERIAL_H

#include <any>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

#include "resources/resourceManager.h"
#include "utils/logging/log.h"

using MaterialProperty = std::variant<int, float, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

class Material {
private:
    std::unordered_map<std::string, uint32_t> textures;
    std::unordered_map<std::string, MaterialProperty> properties;
    uint32_t shader{};

public:
    void addTexture(const std::string &name, const std::string &path);
    void setTexture(const std::string &name, const std::string &path);
    void addTexture(const std::string &name);
    [[nodiscard]] std::unordered_map<std::string, uint32_t> getTextures() const { return textures; }
    void removeTexture(const std::string &name) { textures.erase(name); }

    void addShader(const std::string &name, Shader &&shaderObj);
    void setShader(const std::string &name, Shader &&shaderObj);
    void addShader(const std::string &name);
    [[nodiscard]] uint32_t getShader() const { return shader; }
    void removeShader() { shader = 0; }

    template <typename T>
    void addProperty(const std::string& name, T object) {
            if (properties.contains(name)) {
                engineLogger("stdError", "\"" + name + "\" already exists.");
                return;
            }

        properties[name] = object;
    }

    template <typename T>
    void setProperty(const std::string& name, T object) {
        properties[name] = object;
    }

    [[nodiscard]] std::unordered_map<std::string, MaterialProperty> getProperties() const { return properties; }

    void removeProperty(const std::string &name) { properties.erase(name); }

    static Material createStandardPBR();
};

#endif //LSIM_MATERIAL_H
