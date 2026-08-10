#include <resources/resourceManager.h>

#include "gl/shaderClass.h"
#include "utils/texture.h"

std::unordered_map<std::string, uint32_t> ResourceManager::textures;
std::unordered_map<std::string, Shader> ResourceManager::shaders;
std::unordered_map<uint32_t, Shader*> ResourceManager::shadersById;

uint32_t ResourceManager::addTexture(const std::string &name, const std::string &path) {
    const uint32_t tex = Texture::GetTexId(path.c_str(), GL_NEAREST);
    textures[name] = tex;
    return tex;
}

Shader& ResourceManager::addShader(const std::string &name, Shader &&shaderObj) {
    auto [it, inserted] = shaders.emplace(name, std::move(shaderObj));
    if (inserted)
        shadersById[it->second.GetID()] = &it->second;
    return it->second;
}

Shader *ResourceManager::getShaderById(const uint32_t id) {
    const auto it = shadersById.find(id);
    return it != shadersById.end() ? it->second : nullptr;
}
