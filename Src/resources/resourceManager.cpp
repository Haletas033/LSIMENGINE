#include <expected>
#include <resources/resourceManager.h>

#include "utils/fileIO.h"
#include "utils/texture.h"
#include "vk/device.h"
#include "vk/vk.h"

std::unordered_map<std::string, TextureResource> ResourceManager::textures;
std::unordered_map<std::string, VkShaderModule> ResourceManager::shaderModules;
std::unordered_map<std::string, Pipeline> ResourceManager::pipelines;

std::expected<TextureResource*, LSIM::Error> ResourceManager::getTexture(const std::string& name) {
        const auto it = textures.find(name);

        if (it == textures.end())
                return std::unexpected<LSIM::Error>(
                        {LSIM::ErrorCode::LSIM_NO_TEXTURE_FOUND, LSIM::FatalityLevel::FATAL});

        return &it->second;
}

std::expected<TextureResource*, LSIM::Error> ResourceManager::addTexture(
        const std::string& name, const std::string& path, const VkFilter filter) {
        auto tex = Texture::loadTexture(path.c_str(), filter);
        if (!tex) return std::unexpected(tex.error());
        textures[name] = std::move(*tex);
        return &textures[name];
}

VkShaderModule*
ResourceManager::addShaderModule(const Device& device, const std::string& name, const std::string& path) {
        const std::string code = IO::GetFileContents(path);

        const VkShaderModuleCreateInfo shaderModuleCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = code.size(),
                .pCode = reinterpret_cast<const uint32_t*>(code.data())
        };

        vkCreateShaderModule(
                device.getLogicalDevice(),
                &shaderModuleCreateInfo,
                nullptr,
                &shaderModules[name]
        );

        return &shaderModules[name];
}

VkShaderModule* ResourceManager::getShaderModule(const std::string& name) {
        const auto it = shaderModules.find(name);

        if (it == shaderModules.end())
                return nullptr;

        return &it->second;
}

Pipeline* ResourceManager::addPipeline(const std::string& name, Pipeline pipeline) {
        pipelines[name] = std::move(pipeline);
        return &pipelines[name];
}

Pipeline* ResourceManager::getPipeline(const std::string& name) {
        const auto it = pipelines.find(name);

        if (it == pipelines.end())
                return nullptr;

        return &it->second;
}

void ResourceManager::clear(const Device& device) {
        textures.clear();

        for (const auto& shader : shaderModules | std::views::values) {
                vkDestroyShaderModule(device.getLogicalDevice(), shader, nullptr);
        }
        shaderModules.clear();

        pipelines.clear();
}
