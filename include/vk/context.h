#ifndef LSIM_CONTEXT_H
#define LSIM_CONTEXT_H

#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <expected>

#include <vulkan/vulkan_core.h>

#include "device.h"
#include "LSIMtypes.h"
#include "renderPass.h"
#include "swapchain.h"

struct GLFWwindow;
class Context {
private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        Device device{};
        Swapchain swapchain{};
        RenderPass renderPass{};
        static constexpr const char* VK_VALIDATION_LAYERS[] = {"VK_LAYER_KHRONOS_validation"};

public:
        static constexpr auto APPLICATION_NAME = "TODO: Application name";
        static constexpr uint32_t APPLICATION_VERSION = VK_MAKE_VERSION(0, 0, 0);
        static constexpr auto ENGINE_NAME = "LSIM";
        static constexpr uint32_t ENGINE_VERSION = VK_MAKE_VERSION(1, 2, 0);

private:
        Context() = default;
public:
        Context(Context&& other) noexcept;
        Context& operator=(Context&& other) noexcept;

        static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                               VkDebugUtilsMessageTypeFlagsEXT messageType,
                               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        static std::expected<std::vector<const char *>, LSIM::Error> getExtensions();

        static std::expected<VkInstance, LSIM::Error> createInstance();

        static std::expected<VkDebugUtilsMessengerEXT, LSIM::Error> createDebugMessenger(const VkInstance &instance);

        void destroyDebugMessenger();

        static std::expected<Context, LSIM::Error> create(GLFWwindow *window);

        void destroy();

        ~Context();

};

#endif //LSIM_CONTEXT_H
