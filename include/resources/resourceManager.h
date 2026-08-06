#ifndef LSIM_RESOURCEMANAGER_H
#define LSIM_RESOURCEMANAGER_H
#include <any>
#include <cstdint>
#include <string>
#include <unordered_map>

class Shader;

class ResourceManager {
private:
    static std::unordered_map<std::string, uint32_t> textures;

    static std::unordered_map<std::string, uint32_t> shaders;
public:
    static std::unordered_map<std::string, uint32_t> getTextures() { return textures; }
    static uint32_t addTexture(const std::string &name, const std::string &path);

    static std::unordered_map<std::string, uint32_t> getShaders() { return shaders; }
    static uint32_t addShader(const std::string &name, const Shader &shaderObj);
};

#endif //LSIM_RESOURCEMANAGER_H
