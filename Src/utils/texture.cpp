#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include "utils/texture.h"

#include "vk/buffer.h"
#include "vk/vk.h"

TextureResource::TextureResource(TextureResource&& other) noexcept
        : device(other.device),
          image(other.image),
          memory(other.memory),
          imageView(other.imageView),
          sampler(other.sampler),
          width(other.width),
          height(other.height),
          channels(other.channels)
{
        other.channels = 0;
        other.height = 0;
        other.width = 0;
        other.sampler = VK_NULL_HANDLE;
        other.imageView = VK_NULL_HANDLE;
        other.memory = VK_NULL_HANDLE;
        other.image = VK_NULL_HANDLE;
        other.device = VK_NULL_HANDLE;
}

TextureResource& TextureResource::operator=(TextureResource&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        image = other.image;
        memory = other.memory;
        imageView = other.imageView;
        sampler = other.sampler;
        width = other.width;
        height = other.height;
        channels = other.channels;

        other.channels = 0;
        other.height = 0;
        other.width = 0;
        other.sampler = VK_NULL_HANDLE;
        other.imageView = VK_NULL_HANDLE;
        other.memory = VK_NULL_HANDLE;
        other.image = VK_NULL_HANDLE;
        other.device = VK_NULL_HANDLE;

        return *this;
}

std::expected<VkFormat, LSIM::Error> Texture::channelsToFormat(const int nrChannels, TextureType type) {
        switch (nrChannels) {
        case 1:
                return type == TextureType::COLOR ? VK_FORMAT_R8_SRGB : VK_FORMAT_R8_UNORM;
        case 2: {
                if (type == TextureType::COLOR) {
                        return std::unexpected(
                                LSIM::Error{
                                        LSIM::ErrorCode::VK_INCOMPATIBLE_FORMAT_FOR_TYPE,
                                        LSIM::FatalityLevel::FATAL
                                }
                        );
                }
                return VK_FORMAT_R8G8_UNORM;
        }
        case 4:
                return type == TextureType::COLOR ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
        default:
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::VK_NO_VALID_IMAGE_FORMAT,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }
}

std::expected<TextureResource, LSIM::Error> Texture::loadTexture(
        const Device& device,
        Command& command,
        const char* path,
        VkFilter filter,
        TextureType type
) {
        int width, height, channels;
        unsigned char* data = nullptr;
        if (!stbi_info(path, &width, &height, &channels)) {
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::LSIM_GET_IMAGE_INFO_FAILURE,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }
        if (channels == 3) {
                data = stbi_load(path, &width, &height, &channels, 4);
                channels = 4;
        } else {
                data = stbi_load(path, &width, &height, &channels, 0);
        }

        auto imageBuffer = Buffer::create(
                device,
                width * height * channels,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        if (!imageBuffer) {
                stbi_image_free(data);
                return std::unexpected(imageBuffer.error());
        }

        auto res = imageBuffer->upload(data);
        if (!res) return std::unexpected(res.error());
        stbi_image_free(data);

        auto format = channelsToFormat(channels, type);
        if (!format) return std::unexpected(format.error());

        VkImageCreateInfo imageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = *format,
                .extent = VkExtent3D{static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1},
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        TextureResource texture{};
        texture.device = device.getLogicalDevice();
        texture.width = width;
        texture.height = height;
        texture.channels = channels;
        VK_CHECK(
                vkCreateImage(
                        device.getLogicalDevice(),
                        &imageCreateInfo,
                        nullptr,
                        &texture.image
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_FAILURE
        );

        VkMemoryRequirements imageMemoryRequirements{};
        vkGetImageMemoryRequirements(device.getLogicalDevice(), texture.image, &imageMemoryRequirements);

        auto memoryType = device.getMemoryType(imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (!memoryType) return std::unexpected(memoryType.error());

        const VkMemoryAllocateInfo memoryAllocateInfo {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = imageMemoryRequirements.size,
                .memoryTypeIndex = *memoryType
        };

        VK_CHECK(
                vkAllocateMemory(
                        device.getLogicalDevice(),
                        &memoryAllocateInfo,
                        nullptr,
                        &texture.memory
                ),
                LSIM::ErrorCode::VK_MEMORY_ALLOCATION_FAILURE
        );

        VK_CHECK(
                vkBindImageMemory(
                        device.getLogicalDevice(),
                        texture.image,
                        texture.memory,
                        0
                ),
                LSIM::ErrorCode::VK_BIND_IMAGE_MEMORY_FAILURE
        );

        VkCommandBuffer imageCommandBuffer{};
        auto beginOneTimeBufferRes = command.beginOneTimeBuffer(imageCommandBuffer);
        if (!beginOneTimeBufferRes) return std::unexpected(beginOneTimeBufferRes.error());

        VkImageSubresourceRange imageSubresourceRange {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
        };

        VkImageMemoryBarrier imageMemoryBarrier {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = texture.image,
                .subresourceRange = imageSubresourceRange
        };

        vkCmdPipelineBarrier(
                imageCommandBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageMemoryBarrier
        );

        VkImageSubresourceLayers imageSubresourceLayer {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
        };

        VkBufferImageCopy bufferImageCopy {
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource =imageSubresourceLayer ,
                .imageOffset = {0,0,0},
                .imageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}
        };

        vkCmdCopyBufferToImage(
                imageCommandBuffer,
                imageBuffer->getBuffer(),
                texture.image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &bufferImageCopy
        );

        imageMemoryBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = texture.image,
                .subresourceRange = imageSubresourceRange
        };

        vkCmdPipelineBarrier(
                imageCommandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageMemoryBarrier
        );

        auto endOneTimeBufferRes = command.endOneTimeBuffer(imageCommandBuffer, device.getGraphicsQueue());
        if (!endOneTimeBufferRes) return std::unexpected(endOneTimeBufferRes.error());

        VkImageViewCreateInfo imageViewCreateInfo {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = texture.image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = *format,
                .components = {},
                .subresourceRange = imageSubresourceRange
        };

        VK_CHECK(
                vkCreateImageView(
                        device.getLogicalDevice(),
                        &imageViewCreateInfo,
                        nullptr,
                        &texture.imageView
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_VIEW_FAILURE
        );

        VkSamplerCreateInfo samplerCreateInfo {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter = filter,
                .minFilter = filter,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0.f,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0.f,
                .maxLod = 0.f,
                .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE
        };

        VK_CHECK(
                vkCreateSampler(
                        device.getLogicalDevice(),
                        &samplerCreateInfo,
                        nullptr,
                        &texture.sampler
                ),
                LSIM::ErrorCode::VK_CREATE_SAMPLER_FAILURE
        );

        return texture;
}

std::expected<TextureResource, LSIM::Error> Texture::loadCubemap(
        const Device& device,
        Command& command,
        const std::array<std::string, 6>& faces,
        VkFilter filter,
        TextureType type
) {
        int width, height, channels;

        std::vector<unsigned char> cubemapData;
        std::array<unsigned char*, 6> faceData{};
        for (int i = 0; i < faces.size(); ++i) {

                if (!stbi_info(faces[i].c_str(), &width, &height, &channels)) {
                        return std::unexpected(
                                LSIM::Error{
                                        LSIM::ErrorCode::LSIM_GET_IMAGE_INFO_FAILURE,
                                        LSIM::FatalityLevel::FATAL
                                }
                        );
                }
                if (channels == 3) {
                        faceData[i] = stbi_load(faces[i].c_str(), &width, &height, &channels, 4);
                        channels = 4;
                } else {
                        faceData[i] = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
                }
        }

        auto imageBuffer = Buffer::create(
                device,
                width * height * channels * 6,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        if (!imageBuffer) {
                for (auto* data : faceData) {
                        stbi_image_free(data);
                }
                return std::unexpected(imageBuffer.error());
        }

        const VkDeviceSize faceSize =
                static_cast<VkDeviceSize>(width) *
                static_cast<VkDeviceSize>(height) *
                static_cast<VkDeviceSize>(channels);

        const VkDeviceSize totalSize = faceSize * 6;

        cubemapData.resize(totalSize);

        for (size_t i = 0; i < 6; ++i) {
                std::memcpy(
                    cubemapData.data() + i * faceSize,
                    faceData[i],
                    faceSize
                );

                stbi_image_free(faceData[i]);
                faceData[i] = nullptr;
        }

        auto res = imageBuffer->upload(cubemapData.data());
        if (!res) return std::unexpected(res.error());

        auto format = channelsToFormat(channels, type);
        if (!format) return std::unexpected(format.error());

        VkImageCreateInfo imageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = *format,
                .extent = VkExtent3D{static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1},
                .mipLevels = 1,
                .arrayLayers = 6,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        TextureResource texture{};
        texture.device = device.getLogicalDevice();
        texture.width = width;
        texture.height = height;
        texture.channels = channels;
        VK_CHECK(
                vkCreateImage(
                        device.getLogicalDevice(),
                        &imageCreateInfo,
                        nullptr,
                        &texture.image
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_FAILURE
        );

        VkMemoryRequirements imageMemoryRequirements{};
        vkGetImageMemoryRequirements(device.getLogicalDevice(), texture.image, &imageMemoryRequirements);

        auto memoryType = device.getMemoryType(imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (!memoryType) return std::unexpected(memoryType.error());

        const VkMemoryAllocateInfo memoryAllocateInfo {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = imageMemoryRequirements.size,
                .memoryTypeIndex = *memoryType
        };

        VK_CHECK(
                vkAllocateMemory(
                        device.getLogicalDevice(),
                        &memoryAllocateInfo,
                        nullptr,
                        &texture.memory
                ),
                LSIM::ErrorCode::VK_MEMORY_ALLOCATION_FAILURE
        );

        VK_CHECK(
                vkBindImageMemory(
                        device.getLogicalDevice(),
                        texture.image,
                        texture.memory,
                        0
                ),
                LSIM::ErrorCode::VK_BIND_IMAGE_MEMORY_FAILURE
        );

        VkCommandBuffer imageCommandBuffer{};
        auto beginOneTimeBufferRes = command.beginOneTimeBuffer(imageCommandBuffer);
        if (!beginOneTimeBufferRes) return std::unexpected(beginOneTimeBufferRes.error());

        VkImageSubresourceRange imageSubresourceRange {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 6
        };

        VkImageMemoryBarrier imageMemoryBarrier {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = texture.image,
                .subresourceRange = imageSubresourceRange
        };

        vkCmdPipelineBarrier(
                imageCommandBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageMemoryBarrier
        );

        VkImageSubresourceLayers imageSubresourceLayer {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 6
        };

        VkBufferImageCopy bufferImageCopy {
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource =imageSubresourceLayer ,
                .imageOffset = {0,0,0},
                .imageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1}
        };

        vkCmdCopyBufferToImage(
                imageCommandBuffer,
                imageBuffer->getBuffer(),
                texture.image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &bufferImageCopy
        );

        imageMemoryBarrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = texture.image,
                .subresourceRange = imageSubresourceRange
        };

        vkCmdPipelineBarrier(
                imageCommandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &imageMemoryBarrier
        );

        auto endOneTimeBufferRes = command.endOneTimeBuffer(imageCommandBuffer, device.getGraphicsQueue());
        if (!endOneTimeBufferRes) return std::unexpected(endOneTimeBufferRes.error());

        VkImageViewCreateInfo imageViewCreateInfo {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = texture.image,
                .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
                .format = *format,
                .components = {},
                .subresourceRange = imageSubresourceRange
        };

        VK_CHECK(
                vkCreateImageView(
                        device.getLogicalDevice(),
                        &imageViewCreateInfo,
                        nullptr,
                        &texture.imageView
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_VIEW_FAILURE
        );

        VkSamplerCreateInfo samplerCreateInfo {
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter = filter,
                .minFilter = filter,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias = 0.f,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0.f,
                .maxLod = 0.f,
                .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE
        };

        VK_CHECK(
                vkCreateSampler(
                        device.getLogicalDevice(),
                        &samplerCreateInfo,
                        nullptr,
                        &texture.sampler
                ),
                LSIM::ErrorCode::VK_CREATE_SAMPLER_FAILURE
        );

        return texture;
}

void Texture::ByteArrayToPNG(const char* filename, const unsigned char* texture,
                             const int width, const int height, const int channels) {
        stbi_write_png(filename, width, height, channels, texture, width * channels);
}

void TextureResource::destroy() {
        if (sampler != VK_NULL_HANDLE) {
                vkDestroySampler(device, sampler, nullptr);
                sampler = VK_NULL_HANDLE;
        }

        if (imageView != VK_NULL_HANDLE) {
                vkDestroyImageView(device, imageView, nullptr);
                imageView = VK_NULL_HANDLE;
        }

        if (memory != VK_NULL_HANDLE) {
                vkFreeMemory(device, memory, nullptr);
                memory = VK_NULL_HANDLE;
        }

        if (image != VK_NULL_HANDLE) {
                vkDestroyImage(device, image, nullptr);
                image = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

TextureResource::~TextureResource() {
        destroy();
}