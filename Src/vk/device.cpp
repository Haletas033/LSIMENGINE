#include "vk/device.h"

#include <expected>
#include <set>
#include <vector>

#include "LSIMtypes.h"
#include "vk/vk.h"

Device::Device(Device&& other) noexcept
    : device(other.device),
      logicalDevice(other.logicalDevice)
{
        other.device = VK_NULL_HANDLE;
        other.logicalDevice = VK_NULL_HANDLE;
}

Device& Device::operator=(Device&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        logicalDevice = other.logicalDevice;

        other.device = VK_NULL_HANDLE;
        other.logicalDevice = VK_NULL_HANDLE;

        return *this;
}

std::expected<PhysicalDeviceData, LSIM::Error> Device::pickPhysicalDevice(
        const VkInstance &instance, const VkSurfaceKHR &surface) {
        PhysicalDeviceData result{};
        uint32_t deviceCount{};
        VK_CHECK(
                vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr),
                LSIM::ErrorCode::VK_ENUMERATE_PHYSICAL_DEVICES_FAILURE
        );

        std::vector<VkPhysicalDevice> devices{};
        devices.resize(deviceCount);

        VK_CHECK(
                vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()),
                LSIM::ErrorCode::VK_ENUMERATE_PHYSICAL_DEVICES_FAILURE
        );

        VkPhysicalDevice bestDevice{};
        uint32_t bestScore{};
        for (const auto& device : devices) {
                VkPhysicalDeviceProperties properties{};
                vkGetPhysicalDeviceProperties(device, &properties);
                uint32_t score{};
                switch (properties.deviceType) {
                        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                                score = 5;
                                break;
                        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
                                score = 4;
                                break;
                        }

                        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
                                score = 3;
                                break;
                        }

                        case VK_PHYSICAL_DEVICE_TYPE_CPU: {
                                score = 2;
                                break;
                        }

                        case VK_PHYSICAL_DEVICE_TYPE_OTHER: {
                                score = 1;
                                break;
                        }

                        default:
                                break;
                }

                if (score > bestScore) {
                        uint32_t propertyCount{};
                        vkGetPhysicalDeviceQueueFamilyProperties(device, &propertyCount, nullptr);
                        std::vector<VkQueueFamilyProperties> queueFamilyProperties{};
                        queueFamilyProperties.resize(propertyCount);
                        vkGetPhysicalDeviceQueueFamilyProperties(device, &propertyCount, queueFamilyProperties.data());

                        bool foundGraphicsQueueFamily = false;
                        for (uint32_t i{}; i < queueFamilyProperties.size(); ++i) {
                                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                                        foundGraphicsQueueFamily = true;
                                        result.graphicsQueueFamily = i;
                                        break;
                                }
                        }

                        bool foundPresentQueueFamily = false;
                        for (uint32_t i{}; i < queueFamilyProperties.size(); ++i) {
                                VkBool32 hasSurfaceSupport = false;

                                VK_CHECK(
                                        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &hasSurfaceSupport),
                                        LSIM::ErrorCode::VK_GET_PHYSICAL_DEVICE_SURFACE_SUPPORT_FAILURE
                                );

                                if (hasSurfaceSupport) {
                                        foundPresentQueueFamily = true;
                                        result.presentQueueFamily = i;
                                        break;
                                }
                        }

                        if (foundGraphicsQueueFamily && foundPresentQueueFamily) {
                                bestScore = score;
                                bestDevice = device;
                        }
                }
        }

        if (bestDevice == VK_NULL_HANDLE) {
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::VK_NO_SUITABLE_DEVICE_FOUND,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }

        result.device = bestDevice;
        return result;
}

std::expected<VkDevice, LSIM::Error> Device::createLogicalDevice(const PhysicalDeviceData physicalDeviceData) {
        VkDevice device{};
        std::vector<VkDeviceQueueCreateInfo> deviceQueues{};

        constexpr float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo graphicsQueue{};
        graphicsQueue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueue.queueCount = 1;
        graphicsQueue.queueFamilyIndex = physicalDeviceData.graphicsQueueFamily;
        graphicsQueue.pQueuePriorities = &queuePriority;
        deviceQueues.push_back(graphicsQueue);

        if (physicalDeviceData.graphicsQueueFamily != physicalDeviceData.presentQueueFamily) {
                VkDeviceQueueCreateInfo presentQueue{};
                presentQueue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                presentQueue.queueCount = 1;
                presentQueue.queueFamilyIndex = physicalDeviceData.presentQueueFamily;
                presentQueue.pQueuePriorities = &queuePriority;
                deviceQueues.push_back(presentQueue);
        }

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = deviceQueues.size();
        deviceCreateInfo.pQueueCreateInfos = deviceQueues.data();
        deviceCreateInfo.enabledExtensionCount = 1;
        deviceCreateInfo.ppEnabledExtensionNames = VK_SWAPCHAIN_EXTENSION;

        VK_CHECK(
                vkCreateDevice(physicalDeviceData.device, &deviceCreateInfo, nullptr, &device),
                LSIM::ErrorCode::VK_DEVICE_CREATION_FAILURE
        );

        return device;
}

std::expected<Device, LSIM::Error> Device::create(const VkInstance& instance, const VkSurfaceKHR& surface) {
        Device device{};
        const auto physicalDevice = pickPhysicalDevice(instance, surface);
        if (!physicalDevice)
                return std::unexpected(physicalDevice.error());

        device.device = physicalDevice->device;

        const auto logicalDevice = createLogicalDevice(*physicalDevice);
        if (!logicalDevice)
                return std::unexpected(logicalDevice.error());

        device.logicalDevice = *logicalDevice;

        return std::move(device);
}

void Device::destroy() {
        if (logicalDevice != VK_NULL_HANDLE) {
                vkDestroyDevice(logicalDevice, nullptr);
                logicalDevice = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

Device::~Device() {
        destroy();
}
