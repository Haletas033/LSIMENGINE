#ifndef LSIM_RESOURCEMANAGER_H
#define LSIM_RESOURCEMANAGER_H
#include <cstdint>
#include <string>
#include <unordered_map>

#include "gl/shaderClass.h"

class ResourceManager {
private:
    static std::unordered_map<std::string, uint32_t> textures;
    static std::unordered_map<std::string, Shader> shaders;
    static std::unordered_map<uint32_t, Shader*> shadersById;
public:
    static std::unordered_map<std::string, uint32_t> getTextures() { return textures; }
    static uint32_t addTexture(const std::string &name, const std::string &path);

    static std::unordered_map<std::string, Shader>& getShaders() { return shaders; }
    static Shader &addShader(const std::string &name, Shader &&shaderObj);
    static Shader* getShaderById(uint32_t id);

    static void clear() {
        shadersById.clear();
        shaders.clear();
        textures.clear();
    }
};

#endif //LSIM_RESOURCEMANAGER_H
