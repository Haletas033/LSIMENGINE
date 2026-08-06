#include <resources/resourceManager.h>

#include "gl/shaderClass.h"
#include "utils/texture.h"

uint32_t ResourceManager::addTexture(const std::string &name, const std::string &path) {
    const uint32_t tex = Texture::GetTexId(path.c_str(), GL_NEAREST);
    textures[name] = tex;
    return tex;
}

uint32_t ResourceManager::addShader(const std::string &name, const Shader &shaderObj) {
    const uint32_t shader = shaderObj.GetID();
    shaders[name] = shader;
    return shader;
}
