#ifndef LSIM_PIPELINE_H
#define LSIM_PIPELINE_H
#include <expected>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "geometry/meshData.h"
#include "LSIMtypes.h"

enum class BlendFactor {
        OPAQUE,
        TRANSPARENT,
        ADDITIVE
};

struct PipelineConfig {
        VkPrimitiveTopology primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkCullModeFlags cullModeFlags = VK_CULL_MODE_BACK_BIT;
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        VkSampleCountFlagBits sampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT;
        VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
        BlendFactor blendFactor = BlendFactor::OPAQUE;
        bool blendingEnabled = true;
        bool depthTest = true;
        bool depthWrite = true;
};

struct ShaderStage {
        std::string path{};
        VkShaderStageFlagBits stage{};
};

class Pipeline {
private:
        VkDevice device{VK_NULL_HANDLE};
        VkPipeline pipeline{VK_NULL_HANDLE};
        VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};

public:
        Pipeline() = default;

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        Pipeline(Pipeline&& other) noexcept;
        Pipeline& operator=(Pipeline&& other) noexcept;

        static std::expected<Pipeline, LSIM::Error> create(
                VkDevice device,
                VkExtent2D extent,
                VkRenderPass renderPass,
                const std::vector<ShaderStage> &shaderStages,
                PipelineConfig config
        );

        [[nodiscard]] VkPipeline getPipeline() const { return pipeline; }

        void destroy();

        ~Pipeline();
};

#endif //LSIM_PIPELINE_H
