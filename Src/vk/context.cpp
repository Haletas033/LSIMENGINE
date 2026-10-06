#include "vk/context.h"

#include <cstring>
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <chrono>
#include <iomanip>
#include <GLFW/glfw3.h>

#include "vk/device.h"

Context::Context(Context&& other) noexcept
        : instance(other.instance),
          debugMessenger(other.debugMessenger),
          surface(other.surface),
          device(std::move(other.device)),
          swapchain(std::move(other.swapchain)),
          renderPass(std::move(other.renderPass)),
          framebuffers(std::move(other.framebuffers)),
          pipeline(std::move(other.pipeline)),
          command(std::move(other.command)),
          currentFrame(other.currentFrame),
          imagesInFlight(std::move(other.imagesInFlight))
{
        other.instance = VK_NULL_HANDLE;
        other.debugMessenger = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.currentFrame = {};
}

Context& Context::operator=(Context&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        instance = other.instance;
        debugMessenger = other.debugMessenger;
        surface = other.surface;
        device = std::move(other.device);
        swapchain = std::move(other.swapchain);
        renderPass = std::move(other.renderPass);
        framebuffers = std::move(other.framebuffers);
        pipeline = std::move(other.pipeline);
        command = std::move(other.command);
        currentFrame = other.currentFrame;
        imagesInFlight = std::move(other.imagesInFlight);

        other.instance = VK_NULL_HANDLE;
        other.debugMessenger = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.currentFrame = {};

        return *this;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Context::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
) {
        std::cerr << "[Vulkan] " << pCallbackData->pMessage << '\n';
        return VK_FALSE;
}

std::expected<std::vector<const char *>, LSIM::Error> Context::getExtensions() {
        uint32_t extensionCount{};
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        if (glfwExtensions == nullptr) {
                std::cerr << "FATAL: GLFW_GET_EXTENSIONS_FAILURE\n";
                return {
                        std::unexpected(
                                LSIM::Error{
                                        LSIM::ErrorCode::GLFW_GET_EXTENSIONS_FAILURE,
                                        LSIM::FatalityLevel::FATAL
                                }
                        )
                };
        }

        std::vector<const char*> extensions;
        extensions.reserve(extensionCount);

        for (uint32_t i = 0; i < extensionCount; ++i) {
                extensions.emplace_back(glfwExtensions[i]);
        }

        #ifndef NDEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif

        return {extensions};
}

std::expected<VkInstance, LSIM::Error> Context::createInstance() {
        VkInstance instance = VK_NULL_HANDLE;

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = APPLICATION_NAME;
        appInfo.applicationVersion = APPLICATION_VERSION;
        appInfo.pEngineName = ENGINE_NAME;
        appInfo.engineVersion = ENGINE_VERSION;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        auto extension = getExtensions();
        if (!extension) return {std::unexpected(extension.error())};

        createInfo.enabledExtensionCount = extension->size();
        createInfo.ppEnabledExtensionNames = extension->data();

        uint32_t layerCount{};
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> layers{};
        layers.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        #ifndef NDEBUG
        bool foundValidationLayers = false;
        for (const auto& layer : layers) {
                if (std::strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0) foundValidationLayers = true;
        }

        if (!foundValidationLayers) {
                std::cerr << "WARNING: VK_VALIDATION_LAYERS_MISSING\n";
        } else {
                createInfo.enabledLayerCount = 1;
                createInfo.ppEnabledLayerNames = VK_VALIDATION_LAYERS;
        }
        #endif

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
                std::cerr << "FATAL: VK_INSTANCE_CREATION_FAILURE\n";
                return {
                        std::unexpected(
                                LSIM::Error{
                                        LSIM::ErrorCode::VK_INSTANCE_CREATION_FAILURE,
                                        LSIM::FatalityLevel::FATAL
                                }
                        )
                };
        }

        return {instance};
}

std::expected<VkDebugUtilsMessengerEXT, LSIM::Error> Context::createDebugMessenger(const VkInstance& instance) {
        VkDebugUtilsMessengerEXT debugMessenger{};

        auto debugMessengerFP = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
        );

        if (debugMessengerFP == nullptr) {
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::VK_DEBUG_MESSENGER_FUNCTION_POINTER_MISSING,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
        ;

        createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
        ;

        createInfo.pfnUserCallback = debugCallback;

        const VkResult result = debugMessengerFP(
                instance,
                &createInfo,
                nullptr,
                &debugMessenger
        );

        if (result != VK_SUCCESS) {
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::VK_DEBUG_MESSENGER_CREATION_FAILURE,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }

        return debugMessenger;
}

void Context::destroyDebugMessenger() {
        if (debugMessenger == VK_NULL_HANDLE)
                return;

        const auto destroyDebugMessengerFP =
                reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
        );

        if (destroyDebugMessengerFP != nullptr) {
                destroyDebugMessengerFP(instance, debugMessenger, nullptr);
        }

        debugMessenger = VK_NULL_HANDLE;
}

std::expected<Context, LSIM::Error> Context::create(GLFWwindow* window) {
        Context context{};

        auto instance = createInstance();
        if (!instance) return {std::unexpected(instance.error())};
        context.instance = instance.value();

        #ifndef NDEBUG
        auto debugMessenger = createDebugMessenger(context.instance);
        if (!debugMessenger) return std::unexpected(debugMessenger.error());
        context.debugMessenger = debugMessenger.value();
        #endif

        if (const VkResult result = glfwCreateWindowSurface(context.instance, window, nullptr, &context.surface); result != VK_SUCCESS) {
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::VK_SURFACE_CREATION_FAILURE,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }

        if (auto device = Device::create(context.instance, context.surface); !device)
                return std::unexpected(device.error());
        else
                context.device = std::move(*device);

        auto swapchain = Swapchain::create(
                window,
                context.surface,
                context.device.getDevice(),
                context.device
        );
        if (!swapchain)
                return std::unexpected(swapchain.error());
        context.swapchain = std::move(*swapchain);
        context.imagesInFlight.resize(
            context.swapchain.getImagesViews().size(),
            VK_NULL_HANDLE
        );

        auto renderPass = RenderPass::create(
                context.device.getLogicalDevice(),
                context.swapchain
        );
        if (!renderPass)
                return std::unexpected(renderPass.error());
        context.renderPass = std::move(*renderPass);

        context.framebuffers.reserve(context.swapchain.getImagesViews().size());
        for (const auto imageView : context.swapchain.getImagesViews()) {
                auto framebuffer = Framebuffer::create(
                        context.device.getLogicalDevice(),
                        context.renderPass,
                        {imageView, context.swapchain.getDepthImageView()},
                        context.swapchain.getExtent()
                );

                if (!framebuffer)
                        return std::unexpected(framebuffer.error());
                context.framebuffers.push_back(std::move(*framebuffer));
        }

        ShaderStage vertex {
                "shaders/test.vert.spv",
                VK_SHADER_STAGE_VERTEX_BIT
        };

        ShaderStage fragment {
                "shaders/test.frag.spv",
                VK_SHADER_STAGE_FRAGMENT_BIT
        };

        PipelineConfig config {};
        config.cullModeFlags = VK_CULL_MODE_NONE;


        auto pipeline = Pipeline::create(
                context.device.getLogicalDevice(),
                context.swapchain.getExtent(),
                context.renderPass.get(),
                {vertex, fragment},
                config
        );
        if (!pipeline)
                return std::unexpected(pipeline.error());
        context.pipeline = std::move(*pipeline);

        auto command = Command::create(context.device, context.swapchain.getImagesViews().size());
        if (!command)
                return std::unexpected(command.error());
        context.command = std::move(*command);

        return context;
}

std::expected<void, LSIM::Error> Context::render() {
        const auto& logicalDevice = device.getLogicalDevice();
        const auto& buffers = command.getBuffers();
        const auto& imageAvailableSemaphores = command.getImageAvailableSemaphores();
        const auto& renderFinishedSemaphores = command.getRenderFinishedSemaphores();
        const auto& fences = command.getInFlightFences();

        vkWaitForFences(
                logicalDevice,
                1,
                &fences[currentFrame],
                VK_TRUE,
                UINT64_MAX
        );

        uint32_t imageIndex{};
        vkAcquireNextImageKHR(
                logicalDevice,
                swapchain.getSwapchain(),
                UINT64_MAX,
                imageAvailableSemaphores[currentFrame],
                VK_NULL_HANDLE,
                &imageIndex
        );

        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
                vkWaitForFences(
                    logicalDevice,
                    1,
                    &imagesInFlight[imageIndex],
                    VK_TRUE,
                    UINT64_MAX
                );
        }

        imagesInFlight[imageIndex] = fences[currentFrame];

        vkResetFences(
            logicalDevice,
            1,
            &fences[currentFrame]
        );

        vkResetCommandBuffer(
                command.getBuffers()[currentFrame],
                {}
        );

        constexpr VkCommandBufferBeginInfo commandBufferBeginInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        vkBeginCommandBuffer(
                command.getBuffers()[currentFrame],
                &commandBufferBeginInfo
        );

        const VkRect2D rect = {.offset = {0, 0}, .extent = swapchain.getExtent()};
        std::array clearValues{
                VkClearValue{
                        .color = {
                                .float32 = {0.0f, 0.0f, 0.0f, 1.0f}
                        }
                },
                VkClearValue{
                        .depthStencil = {
                                .depth = 1.0f,
                                .stencil = 0
                            }
                }
        };
        const VkRenderPassBeginInfo renderPassBeginInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = renderPass.get(),
                .framebuffer = framebuffers[imageIndex].getFramebuffer(),
                .renderArea = rect,
                .clearValueCount = clearValues.size(),
                .pClearValues = clearValues.data()
        };
        vkCmdBeginRenderPass(
                buffers[currentFrame],
                &renderPassBeginInfo,
                VK_SUBPASS_CONTENTS_INLINE
        );

        vkCmdBindPipeline(
                buffers[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline.getPipeline()
        );

        const VkViewport viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(swapchain.getExtent().width),
                .height = static_cast<float>(swapchain.getExtent().height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f
        };
        vkCmdSetViewport(
                buffers[currentFrame],
                0,1,
                &viewport
        );

        const VkRect2D scissor{
                .offset = {0, 0},
                .extent = swapchain.getExtent()
        };
        vkCmdSetScissor(
                buffers[currentFrame],
                0,
                1,
                &scissor
        );

        vkCmdDraw(
                buffers[currentFrame],
                6, 1, 0, 0
        );

        vkCmdEndRenderPass(buffers[currentFrame]);

        vkEndCommandBuffer(command.getBuffers()[currentFrame]);

        const VkSubmitInfo submitInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &imageAvailableSemaphores[currentFrame],
                .pWaitDstStageMask = WAIT_STAGES,
                .commandBufferCount = 1,
                .pCommandBuffers = &buffers[currentFrame],
                .signalSemaphoreCount = 1,
                .pSignalSemaphores = &renderFinishedSemaphores[imageIndex]
        };
        vkQueueSubmit(
                device.getGraphicsQueue(),
                1,
                &submitInfo,
                fences[currentFrame]
        );

        const VkPresentInfoKHR presentInfo{
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &renderFinishedSemaphores[imageIndex],
                .swapchainCount = 1,
                .pSwapchains = &swapchain.getSwapchain(),
                .pImageIndices = &imageIndex
        };

        vkQueuePresentKHR(
                device.getPresentQueue(),
                &presentInfo
        );

        currentFrame = (currentFrame + 1) % 2;

        return {};
}

void Context::destroy() {
        if (device.getLogicalDevice() != VK_NULL_HANDLE) {
                vkDeviceWaitIdle(device.getLogicalDevice());
        }

        imagesInFlight.clear();
        command = {};
        pipeline = {};
        framebuffers.clear();
        renderPass = {};
        swapchain = {};
        device = {};

        if (surface != VK_NULL_HANDLE) {
                vkDestroySurfaceKHR(instance, surface, nullptr);
                surface = VK_NULL_HANDLE;
        }

        destroyDebugMessenger();

        if (instance != VK_NULL_HANDLE) {
                vkDestroyInstance(instance, nullptr);
                instance = VK_NULL_HANDLE;
        }

        currentFrame = {};
}

Context::~Context() {
        destroy();
}
