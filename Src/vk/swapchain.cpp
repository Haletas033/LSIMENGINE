#include "vk/swapchain.h"

#include "vk/vk.h"

#include <algorithm>
#include <vector>

#include <GLFW/glfw3.h>

std::expected<VkSwapchainKHR, LSIM::Error> Swapchain::create(
        GLFWwindow* window, const VkSurfaceKHR& surface,
        const VkPhysicalDevice& physicalDevice, const VkDevice& device
) {
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};

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

        VkPresentModeKHR bestPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto presentMode : surfacePresentModes) {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                        bestPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        break;
                }
        }

        VkExtent2D swapchainExtent{};

        if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
                swapchainExtent = surfaceCapabilities.currentExtent;
        } else {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                if (width == 0 || height == 0) {
                        std::cout << "FATAL: Swapchain creation called on 0x0 framebuffer. Renderer should skip on minimized windows\n";
                        return std::unexpected(
                            LSIM::Error{
                                LSIM::ErrorCode::VK_FRAMEBUFFER_ZERO_SIZE,
                                LSIM::FatalityLevel::FATAL
                            }
                        );
                }
                VkExtent2D actualExtent{
                        static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height)
                };

                swapchainExtent.width = std::clamp(
                        actualExtent.width,
                        surfaceCapabilities.minImageExtent.width,
                        surfaceCapabilities.maxImageExtent.width
                );

                swapchainExtent.height = std::clamp(
                        actualExtent.height,
                        surfaceCapabilities.minImageExtent.height,
                        surfaceCapabilities.maxImageExtent.height
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
                .imageExtent = swapchainExtent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .preTransform = surfaceCapabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = bestPresentMode,
                .clipped = VK_TRUE,
        };

        VkSwapchainKHR swapchain{};

        VK_CHECK(
                vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain),
                LSIM::ErrorCode::VK_CREATE_SWAPCHAIN_FAILURE
        );

        return swapchain;
}