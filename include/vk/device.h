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

        [[nodiscard]] static std::expected<PhysicalDeviceData, LSIM::Error> pickPhysicalDevice(
                const VkInstance &instance, const VkSurfaceKHR &surface);

        static std::expected<VkDevice, LSIM::Error> createLogicalDevice(PhysicalDeviceData physicalDeviceData);

public:
        VkPhysicalDevice device = VK_NULL_HANDLE;
        VkDevice logicalDevice = VK_NULL_HANDLE;

        Device() = default;

        Device(Device &&other) noexcept;
        Device &operator=(Device &&other) noexcept;

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        static std::expected<Device, LSIM::Error> create(const VkInstance &instance, const VkSurfaceKHR &surface);
        ~Device();
};

#endif //LSIM_DEVICE_H
