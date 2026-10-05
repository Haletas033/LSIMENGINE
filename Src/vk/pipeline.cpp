#include "vk/pipeline.h"

#include "utils/fileIO.h"
#include "vk/vk.h"


Pipeline::Pipeline(Pipeline &&other) noexcept
        : device(other.device),
          pipeline(other.pipeline),
          pipelineLayout(other.pipelineLayout)
{
        other.device = VK_NULL_HANDLE;
        other.pipeline = VK_NULL_HANDLE;
        other.pipelineLayout = VK_NULL_HANDLE;
}

Pipeline &Pipeline::operator=(Pipeline &&other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        pipeline = other.pipeline;
        pipelineLayout = other.pipelineLayout;

        other.device = VK_NULL_HANDLE;
        other.pipeline = VK_NULL_HANDLE;
        other.pipelineLayout = VK_NULL_HANDLE;

        return *this;
}

std::expected<Pipeline, LSIM::Error> Pipeline::create(
        VkDevice device,
        VkExtent2D extent,
        VkRenderPass renderPass,
        const std::vector<ShaderStage>& shaderStages,
        PipelineConfig config
) {
        Pipeline pipeline{};
        pipeline.device = device;

        std::vector<VkShaderModule> shaderModules{};
        shaderModules.reserve(shaderStages.size());
        std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos{};
        pipelineShaderStageCreateInfos.reserve(shaderStages.size());
        for (const auto&[path, stage] : shaderStages) {
                std::string fileData = IO::GetFileContents(path);
                const auto data = reinterpret_cast<const uint32_t *>(fileData.data());

                VkShaderModuleCreateInfo shaderModuleCreateInfo{
                        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                        .codeSize = fileData.size(),
                        .pCode = data
                };

                VkShaderModule shaderModule{};
                VK_CHECK(
                        vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule),
                        LSIM::ErrorCode::VK_CREATE_SHADER_MODULE_FAILURE
                );


                VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .stage = stage,
                        .module = shaderModule,
                        .pName = "main",
                };

                shaderModules.push_back(shaderModule);
                pipelineShaderStageCreateInfos.push_back(pipelineShaderStageCreateInfo);
        }

        VkVertexInputBindingDescription vertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        std::array<VkVertexInputAttributeDescription, 4> vkVertexInputAttributeDescriptions{};

        VkVertexInputAttributeDescription positionAttributeDescription{
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(Vertex, position)
        };

        VkVertexInputAttributeDescription normalAttributeDescription{
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT,
                .offset = offsetof(Vertex, normal)
        };

        VkVertexInputAttributeDescription tangentAttributeDescription{
                .location = 2,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset = offsetof(Vertex, tangent)
        };

        VkVertexInputAttributeDescription UVAttributeDescription{
                .location = 3,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = offsetof(Vertex, uv)
        };

        vkVertexInputAttributeDescriptions[0] = positionAttributeDescription;
        vkVertexInputAttributeDescriptions[1] = normalAttributeDescription;
        vkVertexInputAttributeDescriptions[2] = tangentAttributeDescription;
        vkVertexInputAttributeDescriptions[3] = UVAttributeDescription;

        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &vertexInputBindingDescription,
                .vertexAttributeDescriptionCount = 4,
                .pVertexAttributeDescriptions = vkVertexInputAttributeDescriptions.data()
        };

        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = config.primitiveTopology,
                .primitiveRestartEnable = VK_FALSE
        };

        std::vector dynamicStates {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
                .pDynamicStates = dynamicStates.data()
        };

        VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = 1,
                .scissorCount = 1
        };

        VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = config.polygonMode,
                .cullMode = config.cullModeFlags,
                .frontFace = config.frontFace,
                .depthBiasEnable = VK_FALSE,
                .lineWidth = 1.0f
        };

        VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .rasterizationSamples = config.sampleCountFlagBits,
                .sampleShadingEnable = VK_FALSE,
        };

        VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = config.depthTest,
                .depthWriteEnable = config.depthWrite,
                .depthCompareOp = config.depthCompareOp,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
        };

        VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};


        switch (config.blendFactor) {
                case BlendFactor::OPAQUE: {
                        pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
                        break;
                }
                case BlendFactor::TRANSPARENT: {
                        pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
                        pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                        pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                        pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                        pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                        break;
                }
                case BlendFactor::ADDITIVE: {
                        pipelineColorBlendAttachmentState.blendEnable = VK_TRUE;
                        pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                        pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
                        pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                        pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                        break;
                }
        }

        pipelineColorBlendAttachmentState.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT
                | VK_COLOR_COMPONENT_G_BIT
                | VK_COLOR_COMPONENT_B_BIT
                | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .logicOpEnable = VK_FALSE,
                .logicOp = VK_LOGIC_OP_COPY,
                .attachmentCount = 1,
                .pAttachments = &pipelineColorBlendAttachmentState,
                .blendConstants = {0.f, 0.f, 0.f, 0.f}
        };

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        };

        VK_CHECK(
                vkCreatePipelineLayout(
                        pipeline.device,
                        &pipelineLayoutCreateInfo,
                        nullptr,
                        &pipeline.pipelineLayout
                ),
                LSIM::ErrorCode::VK_CREATE_PIPELINE_LAYOUT_FAILURE
        );

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .stageCount = static_cast<uint32_t>(shaderStages.size()),
                .pStages = pipelineShaderStageCreateInfos.data(),
                .pVertexInputState = &pipelineVertexInputStateCreateInfo,
                .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
                .pViewportState = &pipelineViewportStateCreateInfo,
                .pRasterizationState = &pipelineRasterizationStateCreateInfo,
                .pMultisampleState = &pipelineMultisampleStateCreateInfo,
                .pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
                .pColorBlendState = &pipelineColorBlendStateCreateInfo,
                .pDynamicState = &pipelineDynamicStateCreateInfo,
                .layout = pipeline.pipelineLayout,
                .renderPass = renderPass,
                .subpass = 0,
                .basePipelineHandle = VK_NULL_HANDLE,
                .basePipelineIndex = -1
        };

        VK_CHECK(
                vkCreateGraphicsPipelines(
                        device,
                        VK_NULL_HANDLE,
                        1,
                        &graphicsPipelineCreateInfo,
                        nullptr,
                        &pipeline.pipeline
                ),
                LSIM::ErrorCode::VK_CREATE_GRAPHICS_PIPELINES_FAILURE
        );

        for (auto shaderModule : shaderModules) {
                vkDestroyShaderModule(device, shaderModule, nullptr);
        }

        return pipeline;
}

void Pipeline::destroy() {
        if (pipeline != VK_NULL_HANDLE) {
                vkDestroyPipeline(device, pipeline, nullptr);
                pipeline = VK_NULL_HANDLE;
        }

        if (pipelineLayout != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
                pipelineLayout = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

Pipeline::~Pipeline() {
        destroy();
}

