#ifndef LSIM_SWAPCHAIN_H
#define LSIM_SWAPCHAIN_H

#include <expected>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "LSIMtypes.h"
#include "GLFW/glfw3.h"

class Swapchain {
private:
        VkDevice device = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        VkFormat imageFormat{};
        VkExtent2D extent{};

        std::vector<VkImage> images{};
        std::vector<VkImageView> imageViews{};

public:
        Swapchain() = default;

        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        Swapchain(Swapchain&& other) noexcept;
        Swapchain& operator=(Swapchain&& other) noexcept;

        static std::expected<Swapchain, LSIM::Error> create(GLFWwindow *window, const VkSurfaceKHR &surface,
                                                            const VkPhysicalDevice &physicalDevice,
                                                            const VkDevice &device);

        void destroy();

        ~Swapchain();
};

#endif //LSIM_SWAPCHAIN_H
