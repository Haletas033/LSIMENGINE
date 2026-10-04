#include "vk/context.h"

#include <cstring>
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk/device.h"
#include "vk/swapchain.h"

Context::Context(Context&& other) noexcept
        : instance(other.instance),
          debugMessenger(other.debugMessenger),
          surface(other.surface),
          device(std::move(other.device)),
          swapchain(other.swapchain)
{
        other.instance = VK_NULL_HANDLE;
        other.debugMessenger = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.swapchain = VK_NULL_HANDLE;
}

Context& Context::operator=(Context&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        instance = other.instance;
        debugMessenger = other.debugMessenger;
        surface = other.surface;
        device = std::move(other.device);
        swapchain = other.swapchain;

        other.instance = VK_NULL_HANDLE;
        other.debugMessenger = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.swapchain = VK_NULL_HANDLE;

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

        auto debugMessenger = createDebugMessenger(context.instance);
        if (!debugMessenger) return {std::unexpected(debugMessenger.error())};
        context.debugMessenger = debugMessenger.value();

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

        if (auto swapchain = Swapchain::create(window, context.surface, context.device.device, context.device.logicalDevice); !swapchain)
                return std::unexpected(swapchain.error());
        else
                context.swapchain = *swapchain;

        return context;
}

void Context::destroy() {
        if (swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(device.logicalDevice, swapchain, nullptr);
                swapchain = VK_NULL_HANDLE;
        }

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
}

Context::~Context() {
        destroy();
}
