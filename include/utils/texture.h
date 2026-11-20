#ifndef TEXTURE_H
#define TEXTURE_H

#include <stb_image_write.h>
#include <vulkan/vulkan_core.h>

#include "utils/json.h"
#include "utils/logging/log.h"
#include "vk/command.h"
#include "vk/device.h"


struct TextureResource {
        VkDevice device = VK_NULL_HANDLE;
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;

        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t channels = 0;

        TextureResource() = default;

        TextureResource(const TextureResource&) = delete;
        TextureResource& operator=(const TextureResource&) = delete;

        TextureResource(TextureResource&& other) noexcept;
        TextureResource& operator=(TextureResource&& other) noexcept;

        void destroy();

        ~TextureResource();
};

enum class TextureType {
        COLOR,
        DATA
};

class Texture {
private:
        static std::expected<VkFormat, LSIM::Error> channelsToFormat(int nrChannels, TextureType type);

public:
        static std::expected<TextureResource, LSIM::Error> loadTexture(
                const Device& device,
                Command& command,
                const char* path,
                VkFilter filter, TextureType type
        );

        static std::expected<TextureResource, LSIM::Error> loadCubemap(
                const Device& device,
                Command& command,
                const std::array<std::string, 6>& faces,
                VkFilter filter, TextureType type
        );

        static void ByteArrayToPNG(
                const char* filename,
                const unsigned char* texture,
                int width,
                int height,
                int channels
        );
};

#endif //TEXTURE_H
