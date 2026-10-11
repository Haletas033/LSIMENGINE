#ifndef LSIM_ATTACHMENT_H
#define LSIM_ATTACHMENT_H
#include <expected>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "LSIMtypes.h"

class Attachment {
private:
        VkDevice device{VK_NULL_HANDLE};
        VkFormat format{VK_FORMAT_UNDEFINED};
        VkImage image{VK_NULL_HANDLE};
        VkDeviceMemory memory{VK_NULL_HANDLE};
        VkImageView view{VK_NULL_HANDLE};

public:
        Attachment() = default;

        Attachment(const Attachment&) = delete;
        Attachment& operator=(const Attachment&) = delete;

        Attachment(Attachment&& other) noexcept;
        Attachment& operator=(Attachment&& other) noexcept;
        static std::expected<VkFormat, LSIM::Error> getOptimalDepthBufferFormat(VkPhysicalDevice device);

        static std::expected<Attachment, LSIM::Error> create(
                const Device& device,
                VkFormat format,
                VkExtent2D extent,
                VkImageUsageFlags usage,
                VkImageAspectFlags aspect
        );

        [[nodiscard]] VkFormat getFormat() const { return format; }
        [[nodiscard]] VkImage getImage() const { return image; }
        [[nodiscard]] VkDeviceMemory getMemory() const { return memory; }
        [[nodiscard]] VkImageView getView() const { return view; }

        void destroy();

        ~Attachment();
};

#endif //LSIM_ATTACHMENT_H
