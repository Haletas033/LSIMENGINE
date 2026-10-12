#include "vk/context.h"

#include <cstring>
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <chrono>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "geometry/primitive.h"
#include "vk/attachment.h"
#include "vk/device.h"
#include "vk/ubo.h"
#include "vk/vk.h"

Context::Context(Context&& other) noexcept
        : instance(other.instance),
          debugMessenger(other.debugMessenger),
          surface(other.surface),
          device(std::move(other.device)),
          swapchain(std::move(other.swapchain)),
          attachments(std::move(other.attachments)),

          gbufferRenderPass(std::move(other.gbufferRenderPass)),
          gbufferFramebuffers(std::move(other.gbufferFramebuffers)),
          gbufferPipeline(std::move(other.gbufferPipeline)),
          gbufferDescriptorSet(std::move(other.gbufferDescriptorSet)),

          lightingRenderPass(std::move(other.lightingRenderPass)),
          lightingFramebuffers(std::move(other.lightingFramebuffers)),
          lightingPipeline(std::move(other.lightingPipeline)),
          lightingDescriptorSet(std::move(other.lightingDescriptorSet)),

          command(std::move(other.command)),
          vertexBuffer(std::move(other.vertexBuffer)),
          indexBuffer(std::move(other.indexBuffer)),
          ubo(other.ubo),
          uniformBuffers(std::move(other.uniformBuffers)),
          sampler(other.sampler),
          indexCount(other.indexCount),
          currentFrame(other.currentFrame),
          imagesInFlight(std::move(other.imagesInFlight))
{
        other.instance = VK_NULL_HANDLE;
        other.debugMessenger = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.sampler = VK_NULL_HANDLE;
        other.indexCount = {};
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
        attachments = std::move(other.attachments);

        gbufferRenderPass = std::move(other.gbufferRenderPass);
        gbufferFramebuffers = std::move(other.gbufferFramebuffers);
        gbufferPipeline = std::move(other.gbufferPipeline);
        gbufferDescriptorSet = std::move(other.gbufferDescriptorSet);

        lightingRenderPass = std::move(other.lightingRenderPass);
        lightingFramebuffers = std::move(other.lightingFramebuffers);
        lightingPipeline = std::move(other.lightingPipeline);
        lightingDescriptorSet = std::move(other.lightingDescriptorSet);

        command = std::move(other.command);
        vertexBuffer = std::move(other.vertexBuffer);
        indexBuffer = std::move(other.indexBuffer);

        ubo = other.ubo;
        uniformBuffers = std::move(other.uniformBuffers);
        sampler = other.sampler;

        indexCount = other.indexCount;
        currentFrame = other.currentFrame;
        imagesInFlight = std::move(other.imagesInFlight);

        other.instance = VK_NULL_HANDLE;
        other.debugMessenger = VK_NULL_HANDLE;
        other.surface = VK_NULL_HANDLE;
        other.sampler = VK_NULL_HANDLE;
        other.indexCount = {};
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

        const auto extent = context.swapchain.getExtent();

        // Albedo
        auto albedoAttachment = Attachment::create(
                context.device,
                VK_FORMAT_R8G8B8A8_UNORM,
                extent,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_ASPECT_COLOR_BIT
        );

        if (!albedoAttachment)
                return std::unexpected(albedoAttachment.error());

        context.attachments.push_back(std::move(*albedoAttachment));

        // Normal
        auto normalAttachment = Attachment::create(
                context.device,
                VK_FORMAT_R16G16B16A16_SFLOAT,
                extent,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_ASPECT_COLOR_BIT
        );

        if (!normalAttachment)
                return std::unexpected(normalAttachment.error());

        context.attachments.push_back(std::move(*normalAttachment));

        // Emissive
        auto emissiveAttachment = Attachment::create(
                context.device,
                VK_FORMAT_R8G8B8A8_UNORM,
                extent,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_ASPECT_COLOR_BIT
        );

        if (!emissiveAttachment)
                return std::unexpected(emissiveAttachment.error());

        context.attachments.push_back(std::move(*emissiveAttachment));

        // Depth
        auto depthFormat =
                Attachment::getOptimalDepthBufferFormat(context.device.getDevice());

        if (!depthFormat)
                return std::unexpected(depthFormat.error());

        auto depthAttachment = Attachment::create(
                context.device,
                *depthFormat,
                extent,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_ASPECT_DEPTH_BIT
        );

        if (!depthAttachment)
                return std::unexpected(depthAttachment.error());

        context.attachments.push_back(std::move(*depthAttachment));

        RenderPass::AttachmentDescriptor colorDescriptor {
                .format = VK_FORMAT_R8G8B8A8_UNORM,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .isDepth = false
        };

        RenderPass::AttachmentDescriptor normalDescriptor {
                .format = VK_FORMAT_R16G16B16A16_SFLOAT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .isDepth = false
        };

        RenderPass::AttachmentDescriptor emissiveDescriptor {
                .format = VK_FORMAT_R8G8B8A8_UNORM,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .isDepth = false
        };

        RenderPass::AttachmentDescriptor depthDescriptor {
                .format = context.attachments[3].getFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                .isDepth = true
        };

        std::array descriptors{colorDescriptor, normalDescriptor, emissiveDescriptor, depthDescriptor};

        auto renderPass = RenderPass::create(
                context.device.getLogicalDevice(),
                descriptors
        );
        if (!renderPass)
                return std::unexpected(renderPass.error());
        context.gbufferRenderPass = std::move(*renderPass);

        context.gbufferFramebuffers.reserve(context.swapchain.getImagesViews().size());
        for (const auto imageView : context.swapchain.getImagesViews()) {
                auto framebuffer = Framebuffer::create(
                        context.device.getLogicalDevice(),
                        context.gbufferRenderPass,
                        {
                                context.attachments[0].getView(),
                                context.attachments[1].getView(),
                                context.attachments[2].getView(),
                                context.attachments[3].getView()
                        },
                        context.swapchain.getExtent()
                );

                if (!framebuffer)
                        return std::unexpected(framebuffer.error());
                context.gbufferFramebuffers.push_back(std::move(*framebuffer));
        }

        context.ubo.cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);

        context.ubo.view = glm::lookAt(
                context.ubo.cameraPos,
                glm::vec3(0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
        );

        context.ubo.proj = glm::perspective(
                glm::radians(45.0f),
                static_cast<float>(context.swapchain.getExtent().width) /
                static_cast<float>(context.swapchain.getExtent().height),
                0.1f,
                10000.0f
        );

        context.ubo.proj[1][1] *= -1.0f;

        for (auto& buffer : context.uniformBuffers) {
                auto uniformBuffer = Buffer::create(
                        context.device,
                        sizeof(context.ubo),
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                );

                if (!uniformBuffer)
                        return std::unexpected(uniformBuffer.error());
                buffer = std::move(*uniformBuffer);
        }

        DescriptorSet::BindingDescriptor binding{
                .binding = 0,
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        };

        std::array bindings = {binding};

        VkDescriptorBufferInfo bufferInfo0{
                .buffer = context.uniformBuffers[0].getBuffer(),
                .offset = 0,
                .range = sizeof(UBO)
        };

        VkDescriptorBufferInfo bufferInfo1{
                .buffer = context.uniformBuffers[1].getBuffer(),
                .offset = 0,
                .range = sizeof(UBO)
        };

        DescriptorSet::WriteDescriptor write0{
                .binding = 0,
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .info = bufferInfo0
        };

        DescriptorSet::WriteDescriptor write1{
                .binding = 0,
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .info = bufferInfo1
        };

        std::array gbufferFrame0Writes{write0};
        std::array gbufferFrame1Writes{write1};

        std::array<std::span<const DescriptorSet::WriteDescriptor>, 2> writes{
                std::span{gbufferFrame0Writes},
                std::span{gbufferFrame1Writes}
        };

        auto descriptorSet = DescriptorSet::create(
                context.device.getLogicalDevice(),
                bindings,
                writes
        );

        if (!descriptorSet)
                return std::unexpected(descriptorSet.error());

        context.gbufferDescriptorSet = std::move(*descriptorSet);

        VkSamplerCreateInfo samplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .anisotropyEnable = VK_FALSE,
                .minLod = 0.f,
                .maxLod = 0.f,
        };

        VK_CHECK(
                vkCreateSampler(
                        context.device.getLogicalDevice(),
                        &samplerCreateInfo,
                        nullptr,
                        &context.sampler
                ),
                LSIM::ErrorCode::VK_CREATE_SAMPLER_FAILURE
        );

        ShaderStage vertex {
                "shaders/gbuffer.vert.spv",
                VK_SHADER_STAGE_VERTEX_BIT
        };

        ShaderStage fragment {
                "shaders/gbuffer.frag.spv",
                VK_SHADER_STAGE_FRAGMENT_BIT
        };

        PipelineConfig config {};

        auto gbufferPipeline = Pipeline::create(
                context.device.getLogicalDevice(),
                context.swapchain.getExtent(),
                context.gbufferRenderPass.get(),
                {vertex, fragment},
        {context.gbufferDescriptorSet.getLayout()},
                config
        );
        if (!gbufferPipeline)
                return std::unexpected(gbufferPipeline.error());
        context.gbufferPipeline = std::move(*gbufferPipeline);

        // Lighting
        RenderPass::AttachmentDescriptor presentDescriptor{
                .format = context.swapchain.getImageFormat(),
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .isDepth = false
        };

        std::array lightingDescriptors = {presentDescriptor};

        auto lightingRenderPass = RenderPass::create(context.device.getLogicalDevice(), lightingDescriptors);

        if (!lightingRenderPass)
                return std::unexpected(lightingRenderPass.error());
        context.lightingRenderPass = std::move(*lightingRenderPass);

        for (const auto imageView : context.swapchain.getImagesViews()) {
                auto framebuffer = Framebuffer::create(
                        context.device.getLogicalDevice(),
                        context.lightingRenderPass,
                        {imageView},
                        context.swapchain.getExtent()
                );

                if (!framebuffer)
                        return std::unexpected(framebuffer.error());

                context.lightingFramebuffers.push_back(std::move(*framebuffer));
        }

        std::array lightingBindings{
                DescriptorSet::BindingDescriptor{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT},
                DescriptorSet::BindingDescriptor{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                DescriptorSet::BindingDescriptor{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                DescriptorSet::BindingDescriptor{3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                DescriptorSet::BindingDescriptor{4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
        };

        VkDescriptorImageInfo albedoInfo{
                .sampler = context.sampler,
                .imageView = context.attachments[0].getView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorImageInfo normalInfo{
                .sampler = context.sampler,
                .imageView = context.attachments[1].getView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorImageInfo emissiveInfo{
                .sampler = context.sampler,
                .imageView = context.attachments[2].getView(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkDescriptorImageInfo depthInfo{
                .sampler = context.sampler,
                .imageView = context.attachments[3].getView(),
                .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
        };

        DescriptorSet::WriteDescriptor albedoWrite{
                .binding = 1,
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .info = albedoInfo
        };
        DescriptorSet::WriteDescriptor normalWrite{
                .binding = 2,
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .info = normalInfo
        };
        DescriptorSet::WriteDescriptor emissiveWrite{
                .binding = 3,
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .info = emissiveInfo
        };
        DescriptorSet::WriteDescriptor depthWrite{
                .binding = 4,
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .info = depthInfo
        };

        std::array lightingFrame0Writes{write0, albedoWrite, normalWrite, emissiveWrite, depthWrite};
        std::array lightingFrame1Writes{write1, albedoWrite, normalWrite, emissiveWrite, depthWrite};

        auto lightingDescriptorSet = DescriptorSet::create(
                context.device.getLogicalDevice(),
                lightingBindings,
                {lightingFrame0Writes, lightingFrame1Writes}
        );

        if (!lightingDescriptorSet)
                return std::unexpected(lightingDescriptorSet.error());

        context.lightingDescriptorSet = std::move(*lightingDescriptorSet);

        ShaderStage lightingVertex {
                "shaders/lighting.vert.spv",
                VK_SHADER_STAGE_VERTEX_BIT
        };

        ShaderStage lightingFragment {
                "shaders/lighting.frag.spv",
                VK_SHADER_STAGE_FRAGMENT_BIT
        };

        PipelineConfig lightingConfig {};
        lightingConfig.cullModeFlags = VK_CULL_MODE_NONE;
        lightingConfig.fullscreen = true;
        lightingConfig.colorAttachmentCount = 1;
        lightingConfig.depthTest = false;
        lightingConfig.depthWrite = false;

        auto lightingPipeline = Pipeline::create(
                context.device.getLogicalDevice(),
                context.swapchain.getExtent(),
                context.lightingRenderPass.get(),
                {lightingVertex, lightingFragment},
                {context.lightingDescriptorSet.getLayout()},
                lightingConfig
        );

        if (!lightingPipeline)
                return std::unexpected(lightingPipeline.error());
        context.lightingPipeline = std::move(*lightingPipeline);


        auto command = Command::create(context.device, context.swapchain.getImagesViews().size());
        if (!command)
                return std::unexpected(command.error());
        context.command = std::move(*command);

        const VkRect2D rect = {.offset = {0, 0}, .extent = context.swapchain.getExtent()};
        std::array<VkClearValue, 4> clearValues{};

        clearValues[0].color.float32[0] = 0.0f;
        clearValues[0].color.float32[1] = 0.0f;
        clearValues[0].color.float32[2] = 0.0f;
        clearValues[0].color.float32[3] = 1.0f;

        clearValues[1].color.float32[0] = 0.0f;
        clearValues[1].color.float32[1] = 0.0f;
        clearValues[1].color.float32[2] = 0.0f;
        clearValues[1].color.float32[3] = 1.0f;

        clearValues[2].color.float32[0] = 0.0f;
        clearValues[2].color.float32[1] = 0.0f;
        clearValues[2].color.float32[2] = 0.0f;
        clearValues[2].color.float32[3] = 1.0f;

        clearValues[3].depthStencil.depth = 1.0f;
        clearValues[3].depthStencil.stencil = 0;


        auto [vertices, indices] = Primitive::GenerateTorus();
        context.indexCount = indices.size();

        auto vertexBuffer = Buffer::createFromData(
                context.device,
                context.command,
                vertices.data(),
                vertices.size() * sizeof(Vertex),
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
        if (!vertexBuffer)
                return std::unexpected(vertexBuffer.error());
        context.vertexBuffer = std::move(*vertexBuffer);

        auto indexBuffer = Buffer::createFromData(
                context.device,
                context.command,
                indices.data(),
                indices.size() * sizeof(uint32_t),
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        );
        if (!indexBuffer)
                return std::unexpected(indexBuffer.error());
        context.indexBuffer = std::move(*indexBuffer);

        for (uint32_t i{}; i < context.swapchain.getImagesViews().size(); ++i) {
                constexpr VkCommandBufferBeginInfo commandBufferBeginInfo{
                        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                };
                vkBeginCommandBuffer(
                        context.command.getBuffers()[i],
                        &commandBufferBeginInfo
                );

                const VkRenderPassBeginInfo gbufferRenderPassBeginInfo{
                        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                        .renderPass = context.gbufferRenderPass.get(),
                        .framebuffer = context.gbufferFramebuffers[i].getFramebuffer(),
                        .renderArea = rect,
                        .clearValueCount = clearValues.size(),
                        .pClearValues = clearValues.data()
                };
                vkCmdBeginRenderPass(
                        context.command.getBuffers()[i],
                        &gbufferRenderPassBeginInfo,
                        VK_SUBPASS_CONTENTS_INLINE
                );

                vkCmdBindPipeline(
                        context.command.getBuffers()[i],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        context.gbufferPipeline.getPipeline()
                );

                vkCmdBindDescriptorSets(
                        context.command.getBuffers()[i],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        context.gbufferPipeline.getPipelineLayout(),
                        0,
                        1,
                        &context.gbufferDescriptorSet.getSets()[i % 2],
                        0,
                        nullptr
                );

                const VkViewport viewport{
                        .x = 0.0f,
                        .y = 0.0f,
                        .width = static_cast<float>(context.swapchain.getExtent().width),
                        .height = static_cast<float>(context.swapchain.getExtent().height),
                        .minDepth = 0.0f,
                        .maxDepth = 1.0f
                };
                vkCmdSetViewport(
                        context.command.getBuffers()[i],
                        0,1,
                        &viewport
                );

                const VkRect2D scissor{
                        .offset = {0, 0},
                        .extent = context.swapchain.getExtent()
                };
                vkCmdSetScissor(
                        context.command.getBuffers()[i],
                        0,
                        1,
                        &scissor
                );

                const VkBuffer vertexBuffers[] = { context.vertexBuffer.getBuffer() };

                constexpr VkDeviceSize offsets[] = {0};

                vkCmdBindVertexBuffers(
                        context.command.getBuffers()[i],
                        0,
                        1,
                        vertexBuffers,
                        offsets
                );

                vkCmdBindIndexBuffer(
                        context.command.getBuffers()[i],
                        context.indexBuffer.getBuffer(),
                        0,
                        VK_INDEX_TYPE_UINT32
                );

                vkCmdDrawIndexed(
                        context.command.getBuffers()[i],
                        context.indexCount,
                        1,
                        0,
                        0,
                        0
                );

                vkCmdEndRenderPass(context.command.getBuffers()[i]);


                // Lighting
                constexpr VkClearValue lightingClearValue{
                        .color = {{0.0f, 0.0f, 0.0f, 1.0f}}
                };

                const VkRenderPassBeginInfo lightingRenderPassBeginInfo{
                        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                        .renderPass = context.lightingRenderPass.get(),
                        .framebuffer = context.lightingFramebuffers[i].getFramebuffer(),
                        .renderArea = rect,
                        .clearValueCount = 1,
                        .pClearValues = &lightingClearValue
                };

                vkCmdBeginRenderPass(
                        context.command.getBuffers()[i],
                        &lightingRenderPassBeginInfo,
                        VK_SUBPASS_CONTENTS_INLINE
                );

                vkCmdBindPipeline(
                        context.command.getBuffers()[i],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        context.lightingPipeline.getPipeline()
                );

                vkCmdBindDescriptorSets(
                        context.command.getBuffers()[i],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        context.lightingPipeline.getPipelineLayout(),
                        0,
                        1,
                        &context.lightingDescriptorSet.getSets()[i % 2],
                        0,
                        nullptr
                );

                vkCmdDraw(
                        context.command.getBuffers()[i],
                        3, 1, 0, 0
                );

                vkCmdEndRenderPass(context.command.getBuffers()[i]);

                vkEndCommandBuffer(context.command.getBuffers()[i]);
        }

        return context;
}

std::expected<void, LSIM::Error> Context::render() {
        const auto& logicalDevice = device.getLogicalDevice();
        const auto& buffers = command.getBuffers();
        const auto& imageAvailableSemaphores = command.getImageAvailableSemaphores();
        const auto& renderFinishedSemaphores = command.getRenderFinishedSemaphores();
        const auto& fences = command.getInFlightFences();

        const float time = std::chrono::duration<float>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();

        vkWaitForFences(
                logicalDevice,
                1,
                &fences[currentFrame],
                VK_TRUE,
                UINT64_MAX
        );

        ubo.model = glm::mat4(1.0f);

        ubo.model = glm::rotate(
            ubo.model,
            time,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        ubo.model = glm::rotate(
            ubo.model,
            time,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        ubo.model = glm::rotate(
            ubo.model,
            time,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        if (const auto result = uniformBuffers[currentFrame].upload(&ubo); !result)
                return std::unexpected(result.error());

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

        const VkSubmitInfo submitInfo{
                .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &imageAvailableSemaphores[currentFrame],
                .pWaitDstStageMask = WAIT_STAGES,
                .commandBufferCount = 1,
                .pCommandBuffers = &buffers[imageIndex],
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

        vertexBuffer = {};
        indexBuffer = {};

        if (sampler != VK_NULL_HANDLE) {
                vkDestroySampler(device.getLogicalDevice(), sampler, nullptr);
                sampler = VK_NULL_HANDLE;
        }

        uniformBuffers = {};

        command = {};

        lightingPipeline = {};
        lightingFramebuffers.clear();
        lightingRenderPass = {};
        lightingDescriptorSet = {};

        gbufferPipeline = {};
        gbufferFramebuffers.clear();
        gbufferRenderPass = {};
        gbufferDescriptorSet = {};

        attachments.clear();
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

        indexCount = {};
        currentFrame = {};
}

Context::~Context() {
        destroy();
}