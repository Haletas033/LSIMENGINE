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

#define MAX_TEXTURES 4096

class ResourceManager {
private:
        static std::unordered_map<std::string, TextureResource> textures;
        static std::unordered_map<std::string, VkShaderModule> shaderModules;
        static std::unordered_map<std::string, Pipeline> pipelines;
        static VkDescriptorPool bindlessPool;
        static VkDescriptorSetLayout bindlessSetLayout;
        static VkDescriptorSet bindlessSet;
        static uint32_t nextTextureIndex;
        static Device* device;
        static Command* command;

public:
        static void setDevice(Device* newDevice) { device = newDevice; }
        static void setCommand(Command* newCommand) { command = newCommand; }

        static std::unordered_map<std::string, TextureResource>& getTextures() { return textures; }
        static std::expected<void, LSIM::Error> init();
        static std::expected<TextureResource*, LSIM::Error> getTexture(const std::string& name);
        static std::expected<TextureResource*, LSIM::Error> addTexture(
                const std::string& name,
                const std::string& path,
                VkFilter filter,
                TextureType type
        );
        static std::expected<TextureResource*, LSIM::Error> addCubeMapTexture(const std::string& name,
                                                                              const std::array<std::string, 6>& faces,
                                                                              VkFilter filter,
                                                                              TextureType type);

        static std::unordered_map<std::string, VkShaderModule>& getShaderModules() { return shaderModules; }
        static VkShaderModule* getShaderModule(const std::string& name);
        static VkShaderModule* addShaderModule(
                const std::string& name, const std::string& path
        );

        static std::unordered_map<std::string, Pipeline>& getPipelines() { return pipelines; }
        static Pipeline* getPipeline(const std::string& name);
        static Pipeline* addPipeline(
                const std::string& name,
                Pipeline pipeline
        );

        static void clear();
};

#endif //LSIM_RESOURCEMANAGER_H
