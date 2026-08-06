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
        textures[name] = it->second;
    } else {
        textures[name] = ResourceManager::addTexture(name, path);
    }
}

void Material::addTexture(const std::string &name) {
    if (textures.contains(name)) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return;
    }

    const auto &tex = ResourceManager::getTextures();
    if (const auto it = tex.find(name); it != tex.end()) {
        textures[name] = it->second;
    } else {
        engineLogger("stdError", "No texture named: \"" + name + "\" exists.");
    }
}

void Material::addShader(const std::string& name, const Shader& shaderObj) {
    if (shader != 0) {
        engineLogger("stdError", "Material already has a shader.");
        return;
    }

    setShader(name, shaderObj);
}

void Material::setShader(const std::string &name, const Shader& shaderObj) {
    const auto &program = ResourceManager::getShaders();
    if (const auto it = program.find(name); it != program.end()) {
        shader = it->second;
    } else {
        shader = ResourceManager::addShader(name, shaderObj);
    }
}

void Material::addShader(const std::string &name) {
    if (shader != 0) {
        engineLogger("stdError", "\"" + name + "\" already exists.");
        return;
    }

    const auto &program = ResourceManager::getShaders();
    if (const auto it = program.find(name); it != program.end()) {
        shader = it->second;
    } else {
        engineLogger("stdError", "No shader named: \"" + name + "\" exists.");
    }
}
