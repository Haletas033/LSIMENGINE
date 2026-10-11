#include "vk/attachment.h"

#include <array>

#include "vk/device.h"
#include "vk/vk.h"

Attachment::Attachment(Attachment&& other) noexcept
        : device(other.device),
          format(other.format),
          image(other.image),
          memory(other.memory),
          view(other.view)
{
        other.device = VK_NULL_HANDLE;
        other.format = VK_FORMAT_UNDEFINED;
        other.image = VK_NULL_HANDLE;
        other.memory = VK_NULL_HANDLE;
        other.view = VK_NULL_HANDLE;
}

Attachment& Attachment::operator=(Attachment&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        format = other.format;
        image = other.image;
        memory = other.memory;
        view = other.view;

        other.device = VK_NULL_HANDLE;
        other.format = VK_FORMAT_UNDEFINED;
        other.image = VK_NULL_HANDLE;
        other.memory = VK_NULL_HANDLE;
        other.view = VK_NULL_HANDLE;

        return *this;
}

std::expected<VkFormat, LSIM::Error> Attachment::getOptimalDepthBufferFormat(VkPhysicalDevice device) {
        constexpr std::array candidates = {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT
            };

        for (const auto format : candidates) {
                VkFormatProperties props{};
                vkGetPhysicalDeviceFormatProperties(device, format, &props);
                if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                        return format;
        }

        return std::unexpected(
                LSIM::Error{
                        LSIM::ErrorCode::VK_NO_VALID_DEPTH_BUFFER_FORMAT,
                        LSIM::FatalityLevel::FATAL
                }
        );
}

std::expected<Attachment, LSIM::Error> Attachment::create(
        const Device& device,
        const VkFormat format,
        const VkExtent2D extent,
        const VkImageUsageFlags usage,
        const VkImageAspectFlags aspect
) {
        Attachment attachment{};
        attachment.device = device.getLogicalDevice();
        attachment.format = format;

        const VkImageCreateInfo imageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = attachment.format,
                .extent = {extent.width, extent.height, 1},
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = usage,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0,
                .pQueueFamilyIndices = nullptr,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VK_CHECK(
                vkCreateImage(
                        attachment.device,
                        &imageCreateInfo,
                        nullptr,
                        &attachment.image
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_FAILURE
        );

        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(attachment.device, attachment.image, &memoryRequirements);

        auto memoryType = device.getMemoryType(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (!memoryType)
                return std::unexpected(memoryType.error());

        VkMemoryAllocateInfo allocInfo{
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memoryRequirements.size,
                .memoryTypeIndex = *memoryType
        };

        VK_CHECK(
                vkAllocateMemory(attachment.device, &allocInfo, nullptr, &attachment.memory),
                LSIM::ErrorCode::VK_MEMORY_ALLOCATION_FAILURE
        );

        vkBindImageMemory(attachment.device, attachment.image, attachment.memory, 0);

        VkImageViewCreateInfo imageViewCreateInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = attachment.image,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = attachment.format,
                .components = {
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                        .aspectMask = aspect,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                }
        };

        VK_CHECK(
                vkCreateImageView(
                        attachment.device,
                        &imageViewCreateInfo,
                        nullptr,
                        &attachment.view
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_VIEW_FAILURE
        );

        return attachment;
}

void Attachment::destroy() {
        format = VK_FORMAT_UNDEFINED;

        if (view != VK_NULL_HANDLE) {
                vkDestroyImageView(device, view, nullptr);
                view = VK_NULL_HANDLE;
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

Attachment::~Attachment() {
        destroy();
}
