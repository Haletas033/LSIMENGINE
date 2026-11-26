#include <expected>
#include <resources/resourceManager.h>

#include "utils/fileIO.h"
#include "utils/texture.h"
#include "vk/device.h"
#include "vk/vk.h"

std::unordered_map<std::string, TextureResource> ResourceManager::textures{};
std::unordered_map<std::string, VkShaderModule> ResourceManager::shaderModules{};
std::unordered_map<std::string, Pipeline> ResourceManager::pipelines{};
VkDescriptorPool ResourceManager::bindlessPool{};
VkDescriptorSetLayout ResourceManager::bindlessSetLayout{};
VkDescriptorSet ResourceManager::bindlessSet{};
uint32_t ResourceManager::nextTextureIndex{1};

std::expected<void, LSIM::Error> ResourceManager::init() {
        VkDescriptorPoolSize descriptorPoolSize {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = MAX_TEXTURES
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
                .maxSets = 1,
                .poolSizeCount = 1,
                .pPoolSizes = &descriptorPoolSize
        };

        VkDescriptorBindingFlags bindingFlags =
                VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
                .bindingCount = 1,
                .pBindingFlags = &bindingFlags
        };

        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = MAX_TEXTURES,
                .stageFlags = VK_SHADER_STAGE_ALL,
                .pImmutableSamplers = nullptr
        };

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = &bindingFlagsInfo,
                .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
                .bindingCount = 1,
                .pBindings = &descriptorSetLayoutBinding
        };

        VK_CHECK(
                vkCreateDescriptorSetLayout(
                        device->getLogicalDevice(),
                        &descriptorSetLayoutCreateInfo,
                        nullptr,
                        &bindlessSetLayout
                ),
                LSIM::ErrorCode::VK_ALLOCATE_DESCRIPTOR_SETS_FAILURE
        );

        VK_CHECK(
                vkCreateDescriptorPool(
                        device->getLogicalDevice(),
                        &descriptorPoolCreateInfo,
                        nullptr,
                        &bindlessPool
                ),
                LSIM::ErrorCode::VK_CREATE_DESCRIPTOR_POOL_FAILURE
        );

        const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = bindlessPool,
                .descriptorSetCount = 1,
                .pSetLayouts = &bindlessSetLayout
        };

        VK_CHECK(
                vkAllocateDescriptorSets(
                        device->getLogicalDevice(),
                        &descriptorSetAllocateInfo,
                        &bindlessSet
                ),
                LSIM::ErrorCode::VK_ALLOCATE_DESCRIPTOR_SETS_FAILURE
        );

        return {};
}

std::expected<TextureResource*, LSIM::Error> ResourceManager::getTexture(const std::string& name) {
        const auto it = textures.find(name);

        if (it == textures.end())
                return std::unexpected<LSIM::Error>(
                        {LSIM::ErrorCode::LSIM_NO_TEXTURE_FOUND, LSIM::FatalityLevel::FATAL});

        return &it->second;
}

std::expected<TextureResource*, LSIM::Error> ResourceManager::addTexture(
        const std::string& name,
        const std::string& path,
        const VkFilter filter,
        const TextureType type
) {
        auto tex = Texture::loadTexture(*device, *command, path.c_str(), filter, type);
        if (!tex) return std::unexpected(tex.error());

        VkDescriptorImageInfo descriptorImageInfo {
                .sampler = tex->sampler,
                .imageView = tex->imageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkWriteDescriptorSet writeDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = bindlessSet,
                .dstBinding = 0,
                .dstArrayElement = nextTextureIndex,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &descriptorImageInfo,
        };

        vkUpdateDescriptorSets(
                device->getLogicalDevice(),
                1,
                &writeDescriptorSet,
                0,
                nullptr
        );

        tex->bindlessIndex = nextTextureIndex++;
        textures[name] = std::move(*tex);
        return &textures[name];
}

std::expected<TextureResource*, LSIM::Error> ResourceManager::addCubeMapTexture(
        const std::string& name,
        const std::array<std::string, 6>& faces,
        const VkFilter filter,
        const TextureType type
) {
        auto tex = Texture::loadCubemap(*device, *command, faces, filter, type);
        if (!tex) return std::unexpected(tex.error());
        textures[name] = std::move(*tex);
        return &textures[name];
}

VkShaderModule* ResourceManager::addShaderModule(const std::string& name, const std::string& path) {
        const std::string code = IO::GetFileContents(path);

        const VkShaderModuleCreateInfo shaderModuleCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = code.size(),
                .pCode = reinterpret_cast<const uint32_t*>(code.data())
        };

        vkCreateShaderModule(
                device->getLogicalDevice(),
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

void ResourceManager::clear() {
        textures.clear();

        for (const auto& shader : shaderModules | std::views::values) {
                vkDestroyShaderModule(device->getLogicalDevice(), shader, nullptr);
        }
        shaderModules.clear();

        pipelines.clear();
}
