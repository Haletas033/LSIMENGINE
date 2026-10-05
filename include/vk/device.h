#ifndef LSIM_DEVICE_H
#define LSIM_DEVICE_H

#include <expected>
#include <vulkan/vulkan_core.h>

#include "LSIMtypes.h"

struct PhysicalDeviceData {
        VkPhysicalDevice device = VK_NULL_HANDLE;
        uint32_t graphicsQueueFamily{};
        uint32_t presentQueueFamily{};
};

class Device {
private:
        static constexpr const char* VK_SWAPCHAIN_EXTENSION[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkPhysicalDevice physical = VK_NULL_HANDLE;
        VkDevice logical = VK_NULL_HANDLE;

        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        uint32_t graphicsQueueFamily{};
        uint32_t presentQueueFamily{};

        [[nodiscard]] static std::expected<PhysicalDeviceData, LSIM::Error> pickPhysicalDevice(
                const VkInstance &instance, const VkSurfaceKHR &surface);

        static std::expected<VkDevice, LSIM::Error> createLogicalDevice(PhysicalDeviceData physicalDeviceData);

public:
        Device() = default;

        Device(Device &&other) noexcept;
        Device &operator=(Device &&other) noexcept;

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        static std::expected<Device, LSIM::Error> create(const VkInstance &instance, const VkSurfaceKHR &surface);

        [[nodiscard]] VkPhysicalDevice getDevice() const { return physical; }
        [[nodiscard]] VkDevice getLogicalDevice() const { return logical; }
        [[nodiscard]] VkQueue getGraphicsQueue() const { return graphicsQueue; }
        [[nodiscard]] VkQueue getPresentQueue() const { return presentQueue; }
        [[nodiscard]] uint32_t getGraphicsQueueFamily() const { return graphicsQueueFamily; }
        [[nodiscard]] uint32_t getPresentQueueFamily() const { return presentQueueFamily;}
        [[nodiscard]] std::expected<uint32_t, LSIM::Error> getMemoryType(const VkMemoryRequirements &memoryBits,
                                                           VkMemoryPropertyFlags properties) const;

        void destroy();

        ~Device();
};

#endif //LSIM_DEVICE_H
