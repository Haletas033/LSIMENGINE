#ifndef LSIM_SWAPCHAIN_H
#define LSIM_SWAPCHAIN_H

#include <expected>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "LSIMtypes.h"
#include "GLFW/glfw3.h"

class Swapchain {
private:
        VkDevice device = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        VkFormat imageFormat{VK_FORMAT_UNDEFINED};
        VkExtent2D extent{};

        std::vector<VkImage> images{};
        std::vector<VkImageView> imageViews{};

        VkFormat depthFormat{VK_FORMAT_UNDEFINED};
        VkImage depthImage{VK_NULL_HANDLE};
        VkDeviceMemory depthMemory{VK_NULL_HANDLE};
        VkImageView depthImageView{VK_NULL_HANDLE};

public:
        Swapchain() = default;

        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        Swapchain(Swapchain&& other) noexcept;
        Swapchain& operator=(Swapchain&& other) noexcept;

        [[nodiscard]] std::expected<VkFormat, LSIM::Error> getOptimalDepthBufferFormat() const;

        static std::expected<Swapchain, LSIM::Error> create(GLFWwindow *window, const VkSurfaceKHR &surface,
                                                            const VkPhysicalDevice &physicalDevice,
                                                            const Device &device);

        [[nodiscard]] const VkSwapchainKHR& getSwapchain() const { return swapchain; }
        [[nodiscard]] VkFormat getImageFormat() const { return imageFormat; }
        [[nodiscard]] VkFormat getDepthFormat() const { return depthFormat; }
        [[nodiscard]] const std::vector<VkImageView>& getImagesViews() const { return imageViews; }
        [[nodiscard]] VkImageView getDepthImageView() const { return depthImageView; }
        [[nodiscard]] VkExtent2D getExtent() const { return extent; }

        void destroy();

        ~Swapchain();
};

#endif //LSIM_SWAPCHAIN_H
