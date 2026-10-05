#include "vk/swapchain.h"

#include "vk/vk.h"

#include <algorithm>
#include <vector>

#include <GLFW/glfw3.h>

#include "vk/device.h"

Swapchain::Swapchain(Swapchain&& other) noexcept
        : device(other.device),
          physicalDevice(other.physicalDevice),
          swapchain(other.swapchain),
          imageFormat(other.imageFormat),
          extent(other.extent),
          images(std::move(other.images)),
          imageViews(std::move(other.imageViews)),
          depthImage(other.depthImage),
          depthMemory(other.depthMemory),
          depthImageView(other.depthImageView)
{
        other.device = VK_NULL_HANDLE;
        other.physicalDevice = VK_NULL_HANDLE;
        other.swapchain = VK_NULL_HANDLE;
        other.imageFormat = VK_FORMAT_UNDEFINED;
        other.extent = {};
        other.images.clear();
        other.imageViews.clear();
        other.depthImage = VK_NULL_HANDLE;
        other.depthMemory = VK_NULL_HANDLE;
        other.depthImageView = VK_NULL_HANDLE;
}

Swapchain& Swapchain::operator=(Swapchain&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        physicalDevice = other.physicalDevice;
        swapchain = other.swapchain;
        imageFormat = other.imageFormat;
        extent = other.extent;
        images = std::move(other.images);
        imageViews = std::move(other.imageViews);
        depthImage = other.depthImage;
        depthMemory = other.depthMemory;
        depthImageView = other.depthImageView;

        other.device = VK_NULL_HANDLE;
        other.physicalDevice = VK_NULL_HANDLE;
        other.swapchain = VK_NULL_HANDLE;
        other.imageFormat = VK_FORMAT_UNDEFINED;
        other.extent = {};
        other.images.clear();
        other.imageViews.clear();
        other.depthImage = VK_NULL_HANDLE;
        other.depthMemory = VK_NULL_HANDLE;
        other.depthImageView = VK_NULL_HANDLE;

        return *this;
}

std::expected<VkFormat, LSIM::Error> Swapchain::getOptimalDepthBufferFormat() const {
        constexpr std::array candidates = {
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT
            };

        for (const auto format : candidates) {
                VkFormatProperties props{};
                vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
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

std::expected<Swapchain, LSIM::Error> Swapchain::create(
        GLFWwindow *window, const VkSurfaceKHR &surface,
        const VkPhysicalDevice &physicalDevice, const Device &device
) {
        Swapchain swapchain{};
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};

        swapchain.device = device.getLogicalDevice();
        swapchain.physicalDevice = device.getDevice();

        VK_CHECK(
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities),
                LSIM::ErrorCode::VK_GET_PHYSICAL_DEVICE_SURFACE_CAPABILITIES_FAILURE
        );

        uint32_t surfaceFormatCount{};
        std::vector<VkSurfaceFormatKHR> surfaceFormats{};
        VK_CHECK(
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr),
                LSIM::ErrorCode::VK_GET_PHYSICAL_DEVICE_SURFACE_FORMATS_FAILURE
        );
        surfaceFormats.resize(surfaceFormatCount);
        VK_CHECK(
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data()),
                LSIM::ErrorCode::VK_GET_PHYSICAL_DEVICE_SURFACE_FORMATS_FAILURE
        );
        if (surfaceFormats.empty()) {
                return std::unexpected(
                        LSIM::Error{LSIM::ErrorCode::VK_NO_SURFACE_FORMATS, LSIM::FatalityLevel::FATAL}
                );
        }

        uint32_t surfacePresentModeCount{};
        std::vector<VkPresentModeKHR> surfacePresentModes{};
        VK_CHECK(
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, nullptr),
                LSIM::ErrorCode::VK_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES_FAILURE
        );
        surfacePresentModes.resize(surfacePresentModeCount);
        VK_CHECK(
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, surfacePresentModes.data()),
                LSIM::ErrorCode::VK_GET_PHYSICAL_DEVICE_SURFACE_PRESENT_MODES_FAILURE
        );

        VkSurfaceFormatKHR bestSurfaceFormat = surfaceFormats[0];
        for (const auto surfaceFormat : surfaceFormats) {
                if (
                        surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
                        surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                ) {
                        bestSurfaceFormat = surfaceFormat;
                        break;
                }
        }
        swapchain.imageFormat = bestSurfaceFormat.format;

        VkPresentModeKHR bestPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto presentMode : surfacePresentModes) {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                        bestPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        break;
                }
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
                swapchain.extent = surfaceCapabilities.currentExtent;
        } else {

                VkExtent2D actualExtent{
                        static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height)
                };

                swapchain.extent.width = std::clamp(
                        actualExtent.width,
                        surfaceCapabilities.minImageExtent.width,
                        surfaceCapabilities.maxImageExtent.width
                );

                swapchain.extent.height = std::clamp(
                        actualExtent.height,
                        surfaceCapabilities.minImageExtent.height,
                        surfaceCapabilities.maxImageExtent.height
                );
        }

        if (swapchain.extent.width == 0 || swapchain.extent.height == 0) {
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::VK_FRAMEBUFFER_ZERO_SIZE,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }

        uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
        if (
                surfaceCapabilities.maxImageCount > 0 &&
                imageCount > surfaceCapabilities.maxImageCount
        ) {
                imageCount = surfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = surface,
                .minImageCount = imageCount,
                .imageFormat = bestSurfaceFormat.format,
                .imageColorSpace = bestSurfaceFormat.colorSpace,
                .imageExtent = swapchain.extent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .preTransform = surfaceCapabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = bestPresentMode,
                .clipped = VK_TRUE,
        };

        VK_CHECK(
                vkCreateSwapchainKHR(swapchain.device, &swapchainCreateInfo, nullptr, &swapchain.swapchain),
                LSIM::ErrorCode::VK_CREATE_SWAPCHAIN_FAILURE
        );

        uint32_t swapchainImageCount{};
        VK_CHECK(
                vkGetSwapchainImagesKHR(swapchain.device, swapchain.swapchain, &swapchainImageCount,nullptr),
                LSIM::ErrorCode::VK_GET_SWAPCHAIN_IMAGES_FAILURE
        );
        swapchain.images.resize(swapchainImageCount);
        VK_CHECK(
                vkGetSwapchainImagesKHR(swapchain.device, swapchain.swapchain, &swapchainImageCount, swapchain.images.data()),
                LSIM::ErrorCode::VK_GET_SWAPCHAIN_IMAGES_FAILURE
        );

        swapchain.imageViews.reserve(swapchainImageCount);
        for (const auto img : swapchain.images) {
                VkImageViewCreateInfo imageViewCreateInfo {
                        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                        .image = img,
                        .viewType = VK_IMAGE_VIEW_TYPE_2D,
                        .format = bestSurfaceFormat.format,
                        .components = {
                                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .a = VK_COMPONENT_SWIZZLE_IDENTITY
                        },
                        .subresourceRange = {
                                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1
                        }
                };

                VkImageView imageView{};

                VK_CHECK(
                        vkCreateImageView(
                                swapchain.device,
                                &imageViewCreateInfo,
                                nullptr,
                                &imageView
                        ),
                        LSIM::ErrorCode::VK_CREATE_IMAGE_VIEW_FAILURE
                );

                swapchain.imageViews.push_back(imageView);
        }

        auto depthFormat = swapchain.getOptimalDepthBufferFormat();
        if (!depthFormat)
                return std::unexpected(depthFormat.error());

        VkImageCreateInfo imageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = *depthFormat,
                .extent = {swapchain.extent.width, swapchain.extent.height, 1},
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0,
                .pQueueFamilyIndices = nullptr,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VK_CHECK(
                vkCreateImage(
                        swapchain.device,
                        &imageCreateInfo,
                        nullptr,
                        &swapchain.depthImage
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_FAILURE
        );

        VkMemoryRequirements memoryRequirements{};
        vkGetImageMemoryRequirements(swapchain.device, swapchain.depthImage, &memoryRequirements);

        auto memoryType = device.getMemoryType(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (!memoryType)
                return std::unexpected(memoryType.error());

        VkMemoryAllocateInfo allocInfo{
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memoryRequirements.size,
                .memoryTypeIndex = *memoryType
        };

        VK_CHECK(
                vkAllocateMemory(swapchain.device, &allocInfo, nullptr, &swapchain.depthMemory),
                LSIM::ErrorCode::VK_MEMORY_ALLOCATION_FAILURE
        );

        vkBindImageMemory(swapchain.device, swapchain.depthImage, swapchain.depthMemory, 0);

        VkImageViewCreateInfo imageViewCreateInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = swapchain.depthImage,
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = *depthFormat,
                .components = {
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1
                }
        };

        VK_CHECK(
                vkCreateImageView(
                        swapchain.device,
                        &imageViewCreateInfo,
                        nullptr,
                        &swapchain.depthImageView
                ),
                LSIM::ErrorCode::VK_CREATE_IMAGE_VIEW_FAILURE
        );

        return swapchain;
}

void Swapchain::destroy() {
        for (const auto imageView : imageViews)
                vkDestroyImageView(device, imageView, nullptr);

        images.clear();
        imageViews.clear();

        if (depthImageView != VK_NULL_HANDLE) {
                vkDestroyImageView(device, depthImageView, nullptr);
                depthImageView = VK_NULL_HANDLE;
        }

        if (depthMemory != VK_NULL_HANDLE) {
                vkFreeMemory(device, depthMemory, nullptr);
                depthMemory = VK_NULL_HANDLE;
        }

        if (depthImage != VK_NULL_HANDLE) {
                vkDestroyImage(device, depthImage, nullptr);
                depthImage = VK_NULL_HANDLE;
        }

        if (swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(
                        device,
                        swapchain,
                        nullptr
                );

                swapchain = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
        physicalDevice = VK_NULL_HANDLE;
}

Swapchain::~Swapchain() {
        destroy();
}
