#include "vk/renderPass.h"

#include <expected>

#include "vk/swapchain.h"
#include "vk/vk.h"

RenderPass::RenderPass(RenderPass&& other) noexcept
        : device(other.device),
          renderPass(other.renderPass)
{
        other.device = VK_NULL_HANDLE;
        other.renderPass = VK_NULL_HANDLE;
}

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        renderPass = other.renderPass;

        other.device = VK_NULL_HANDLE;
        other.renderPass = VK_NULL_HANDLE;

        return *this;
}

std::expected<RenderPass, LSIM::Error> RenderPass::create(VkDevice device, const Swapchain& swapchain) {
        RenderPass renderPass{};
        renderPass.device = device;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapchain.getImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        const VkRenderPassCreateInfo renderPassCreateInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = 1,
                .pAttachments = &colorAttachment,
                .subpassCount = 1,
                .pSubpasses = &subpass,
        };

        VK_CHECK(
                vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass.renderPass),
                LSIM::ErrorCode::VK_CREATE_RENDER_PASS_FAILURE
        );

        return renderPass;
}

void RenderPass::destroy() {
        if (renderPass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(device, renderPass, nullptr);
                renderPass = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

RenderPass::~RenderPass() {
        destroy();
}
