#ifndef LSIM_RESOURCEMANAGER_H
#define LSIM_RESOURCEMANAGER_H
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "utils/texture.h"
#include "vk/device.h"
#include "vk/pipeline.h"

class ResourceManager {
private:
        static std::unordered_map<std::string, TextureResource> textures;
        static std::unordered_map<std::string, VkShaderModule> shaderModules;
        static std::unordered_map<std::string, Pipeline> pipelines;

public:
        static std::unordered_map<std::string, TextureResource>& getTextures() { return textures; }
        static std::expected<TextureResource*, LSIM::Error> getTexture(const std::string& name);
        static TextureResource* addTexture(
                const std::string& name,
                const std::string& path,
                VkFilter filter = VK_FILTER_NEAREST
        );

        static std::unordered_map<std::string, VkShaderModule>& getShaderModules() { return shaderModules; }
        static VkShaderModule* getShaderModule(const std::string& name);
        static VkShaderModule* addShaderModule(
                const Device& device,
                const std::string& name, const std::string& path
        );

        static std::unordered_map<std::string, Pipeline>& getPipelines() { return pipelines; }
        static Pipeline* getPipeline(const std::string& name);
        static Pipeline* addPipeline(
                const std::string& name,
                Pipeline pipeline
        );

        static void clear(const Device& device);
};

#endif //LSIM_RESOURCEMANAGER_H
