#include "vk/framebuffer.h"

#include "vk/vk.h"

Framebuffer::Framebuffer(Framebuffer &&other) noexcept
        : device(other.device),
          framebuffer(other.framebuffer)
{
        other.device = VK_NULL_HANDLE;
        other.framebuffer = VK_NULL_HANDLE;
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        framebuffer = other.framebuffer;

        other.device = VK_NULL_HANDLE;
        other.framebuffer = VK_NULL_HANDLE;

        return *this;
}

std::expected<Framebuffer, LSIM::Error> Framebuffer::create(
        VkDevice device,
        const RenderPass &renderPass,
        VkImageView imageView,
        const VkExtent2D extent
) {
        Framebuffer framebuffer{};
        framebuffer.device = device;

        const VkFramebufferCreateInfo framebufferCreateInfo{
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = renderPass.get(),
                .attachmentCount = 1,
                .pAttachments = &imageView,
                .width = extent.width,
                .height = extent.height,
                .layers = 1
        };

        VK_CHECK(
                vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer.framebuffer),
                LSIM::ErrorCode::VK_CREATE_FRAMEBUFFER_FAILURE
        );

        return framebuffer;
}

void Framebuffer::destroy() {
        if (framebuffer != VK_NULL_HANDLE) {
                vkDestroyFramebuffer(device, framebuffer, nullptr);
                framebuffer = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

Framebuffer::~Framebuffer() {
        destroy();
}
