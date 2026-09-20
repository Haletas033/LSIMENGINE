#include <rendering/material.h>

#include "gl/shaderClass.h"
#include "utils/logging/log.h"

void Material::addTexture(const std::string &name, const std::string &path) {
    if (textures.contains(name)) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return;
    }

    setTexture(name, path);
}

void Material::setTexture(const std::string &name, const std::string &path) {
    const auto &tex = ResourceManager::getTextures();
    if (const auto it = tex.find(name); it != tex.end()) {
        textures[name] = it->first;
    } else {
        ResourceManager::addTexture(name, path);
        textures[name] = path;
    }
}

void Material::addTexture(const std::string &name) {
    if (textures.contains(name)) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return;
    }

    const auto &tex = ResourceManager::getTextures();
    if (const auto it = tex.find(name); it != tex.end()) {
        textures[name] = it->first;
    } else {
        engineLogger("stdError", "No texture named: \"" + name + "\" exists.");
    }
}

void Material::addShader(const std::string& name, Shader&& shaderObj) {
    if (!shader.empty()) {
        engineLogger("stdError", "Material already has a shader.");
        return;
    }

    setShader(name, std::move(shaderObj));
}

void Material::setShader(const std::string &name, Shader&& shaderObj) {
    const auto& shaders = ResourceManager::getShaders();

    if (!shaders.contains(name)) {
        ResourceManager::addShader(name, std::move(shaderObj));
    }

    shader = name;
}

void Material::addShader(const std::string &name) {
    if (!shader.empty()) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return;
    }

    const auto& shaders = ResourceManager::getShaders();

    if (shaders.contains(name)) {
        shader = name;
    } else {
        engineLogger(
            "stdError",
            "No shader named: \"" + name + "\" exists."
        );
    }
}

Material Material::createStandardPBR() {
    Material mat;
    mat.addShader("PBRShader");
    mat.addProperty("useTexture", 0);
    mat.addProperty("roughness", 0.5f);
    mat.addProperty("F0", 0.03f);
    mat.addProperty("meshColor", glm::vec4(1.f));
    return mat;
}
