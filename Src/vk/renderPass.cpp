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

std::expected<RenderPass, LSIM::Error> RenderPass::create(VkDevice device, const std::span<const AttachmentDescriptor> attachments) {
        RenderPass renderPass{};
        renderPass.device = device;

        std::vector<VkAttachmentDescription> attachmentDescriptions{};
        std::vector<VkAttachmentReference> attachmentRefs{};
        VkAttachmentReference depthAttachmentRef{};
        bool hasDepth = false;
        for (const auto [format, loadOp, storeOp, initialLayout, finalLayout, isDepth] : attachments) {
                VkAttachmentDescription attachmentDescription{};
                attachmentDescription.format = format;
                attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

                attachmentDescription.loadOp = loadOp;
                attachmentDescription.storeOp = storeOp;

                attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                attachmentDescription.initialLayout = initialLayout;
                attachmentDescription.finalLayout = finalLayout;

                const VkImageLayout refLayout = isDepth
                        ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                        : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                const VkAttachmentReference attachmentRef{
                        .attachment = static_cast<uint32_t>(attachmentDescriptions.size()),
                        .layout = refLayout
                };

                attachmentDescriptions.push_back(attachmentDescription);

                if (isDepth) {
                        hasDepth = true;
                        depthAttachmentRef = attachmentRef;
                } else {
                        attachmentRefs.push_back(attachmentRef);
                }
        }

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        subpass.colorAttachmentCount = attachmentRefs.size();
        subpass.pColorAttachments = attachmentRefs.data();

        if (hasDepth)
                subpass.pDepthStencilAttachment = &depthAttachmentRef;

        std::vector<VkSubpassDependency> dependencies{};
        constexpr VkSubpassDependency colorDependency {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = 0
        };
        dependencies.push_back(colorDependency);

        if (hasDepth) {
                constexpr VkSubpassDependency depthDependency {
                        .srcSubpass = VK_SUBPASS_EXTERNAL,
                        .dstSubpass = 0,
                        .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                        .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                        .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                        .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                        .dependencyFlags = 0
                };
                dependencies.push_back(depthDependency);
        }

        const VkRenderPassCreateInfo renderPassCreateInfo{
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size()),
                .pAttachments = attachmentDescriptions.data(),
                .subpassCount = 1,
                .pSubpasses = &subpass,
                .dependencyCount = static_cast<uint32_t>(dependencies.size()),
                .pDependencies = dependencies.data()
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
