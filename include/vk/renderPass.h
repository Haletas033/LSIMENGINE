#ifndef LSIM_RENDERPASS_H
#define LSIM_RENDERPASS_H

#include <expected>
#include <vulkan/vulkan_core.h>

#include "LSIMtypes.h"
#include "swapchain.h"

class RenderPass {
private:
        VkDevice device = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;

public:
        RenderPass() = default;

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;

        RenderPass(RenderPass&& other) noexcept;
        RenderPass& operator=(RenderPass&& other) noexcept;

        static std::expected<RenderPass, LSIM::Error> create(VkDevice device, const Swapchain &swapchain);

        void destroy();

        ~RenderPass();
};

#endif //LSIM_RENDERPASS_H
