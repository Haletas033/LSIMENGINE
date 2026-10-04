#ifndef LSIM_SWAPCHAIN_H
#define LSIM_SWAPCHAIN_H

#include <expected>
#include <vulkan/vulkan_core.h>

#include "context.h"
#include "LSIMtypes.h"
#include "GLFW/glfw3.h"

class Swapchain {
private:
public:
        Swapchain() = default;

        static std::expected<VkSwapchainKHR, LSIM::Error> create(GLFWwindow *window, const VkSurfaceKHR &surface,
                const VkPhysicalDevice &physicalDevice, const VkDevice &device);
};

#endif //LSIM_SWAPCHAIN_H
