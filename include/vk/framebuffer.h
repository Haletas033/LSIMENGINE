#ifndef LSIM_FRAMEBUFFER_H
#define LSIM_FRAMEBUFFER_H

#include <expected>
#include <vulkan/vulkan_core.h>

#include "LSIMtypes.h"
#include "renderPass.h"
#include "swapchain.h"

class Framebuffer {
private:
        VkDevice device = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;

public:
        Framebuffer() = default;

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        Framebuffer(Framebuffer&& other) noexcept;
        Framebuffer& operator=(Framebuffer&& other) noexcept;

        static std::expected<Framebuffer, LSIM::Error> create(
                VkDevice device,
                const RenderPass& renderPass,
                VkImageView imageView,
                VkExtent2D extent
        );

        [[nodiscard]] VkFramebuffer getFramebuffer() const { return framebuffer; }

        void destroy();

        ~Framebuffer();
};

#endif //LSIM_FRAMEBUFFER_H
