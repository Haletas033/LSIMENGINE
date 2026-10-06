#ifndef LSIM_CONTEXT_H
#define LSIM_CONTEXT_H

#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <expected>

#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "command.h"
#include "device.h"
#include "framebuffer.h"
#include "LSIMtypes.h"
#include "pipeline.h"
#include "renderPass.h"
#include "swapchain.h"
#include "ubo.h"
#include "uniformBuffer.h"

struct GLFWwindow;
class Context {
private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        Device device{};
        Swapchain swapchain{};
        RenderPass renderPass{};
        std::vector<Framebuffer> framebuffers{};
        Pipeline pipeline{};
        Command command{};
        Buffer vertexBuffer{};
        Buffer indexBuffer{};
        UBO ubo{};
        UniformBuffer uniformBuffer{};
        uint32_t indexCount{};
        uint32_t currentFrame{};
        std::vector<VkFence> imagesInFlight{};
        static constexpr const char* VK_VALIDATION_LAYERS[] = {"VK_LAYER_KHRONOS_validation"};
        static constexpr VkPipelineStageFlags WAIT_STAGES[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

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

        std::expected<void, LSIM::Error> render();

        void destroy();

        ~Context();

};

#endif //LSIM_CONTEXT_H
