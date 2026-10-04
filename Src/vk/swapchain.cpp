#include "vk/swapchain.h"

#include "vk/vk.h"

#include <algorithm>
#include <vector>

#include <GLFW/glfw3.h>

Swapchain::Swapchain(Swapchain&& other) noexcept
        : device(other.device),
          swapchain(other.swapchain),
          images(std::move(other.images)),
          imageViews(std::move(other.imageViews))
{
        other.device = VK_NULL_HANDLE;
        other.swapchain = VK_NULL_HANDLE;
}

Swapchain& Swapchain::operator=(Swapchain&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        swapchain = other.swapchain;
        images = std::move(other.images);
        imageViews = std::move(other.imageViews);

        other.device = VK_NULL_HANDLE;
        other.swapchain = VK_NULL_HANDLE;

        return *this;
}

std::expected<Swapchain, LSIM::Error> Swapchain::create(
        GLFWwindow *window, const VkSurfaceKHR &surface,
        const VkPhysicalDevice &physicalDevice, const VkDevice &device
) {
        Swapchain swapchain{};
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};

        swapchain.device = device;

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
                vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain.swapchain),
                LSIM::ErrorCode::VK_CREATE_SWAPCHAIN_FAILURE
        );

        uint32_t swapchainImageCount{};
        VK_CHECK(
                vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchainImageCount,nullptr),
                LSIM::ErrorCode::VK_GET_SWAPCHAIN_IMAGES_FAILURE
        );
        swapchain.images.resize(swapchainImageCount);
        VK_CHECK(
                vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchainImageCount, swapchain.images.data()),
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
                                device,
                                &imageViewCreateInfo,
                                nullptr,
                                &imageView
                        ),
                        LSIM::ErrorCode::VK_CREATE_IMAGE_VIEW_FAILURE
                );

                swapchain.imageViews.push_back(imageView);
        }

        return swapchain;
}

void Swapchain::destroy() {
        for (const auto imageView : imageViews)
                vkDestroyImageView(device, imageView, nullptr);

        images.clear();
        imageViews.clear();

        if (swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(
                        device,
                        swapchain,
                        nullptr
                );

                swapchain = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

Swapchain::~Swapchain() {
        destroy();
}
