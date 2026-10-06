#include "vk/uniformBuffer.h"

#include "vk/vk.h"

UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept
        : device(other.device),
          buffers(std::move(other.buffers)),
          descriptorSets(other.descriptorSets),
          descriptorSetLayout(other.descriptorSetLayout),
          descriptorPool(other.descriptorPool)
{
        other.device = VK_NULL_HANDLE;
        other.buffers = {};
        other.descriptorSets.fill(VK_NULL_HANDLE);
        other.descriptorSetLayout = VK_NULL_HANDLE;
        other.descriptorPool = VK_NULL_HANDLE;
}

UniformBuffer &UniformBuffer::operator=(UniformBuffer &&other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        buffers = std::move(other.buffers);
        descriptorSets = other.descriptorSets;
        descriptorSetLayout = other.descriptorSetLayout;
        descriptorPool = other.descriptorPool;

        other.device = VK_NULL_HANDLE;
        other.buffers = {};
        other.descriptorSets.fill(VK_NULL_HANDLE);
        other.descriptorSetLayout = VK_NULL_HANDLE;
        other.descriptorPool = VK_NULL_HANDLE;

        return *this;
}

std::expected<UniformBuffer, LSIM::Error> UniformBuffer::create(const Device &device, void *UBO, size_t size) {
        UniformBuffer uniformBuffer{};
        uniformBuffer.device = device.getLogicalDevice();

        VkDescriptorSetLayoutBinding uboBinding{
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
        };

        VkDescriptorSetLayoutCreateInfo layoutInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = 1,
                .pBindings = &uboBinding
        };

        VK_CHECK(
                vkCreateDescriptorSetLayout(
                        device.getLogicalDevice(),
                        &layoutInfo,
                        nullptr,
                        &uniformBuffer.descriptorSetLayout
                ),
                LSIM::ErrorCode::VK_CREATE_DESCRIPTOR_SET_LAYOUT_FAILURE
        );

        for (auto &ub: uniformBuffer.buffers) {
                auto buffer = Buffer::create(
                        device,
                        size,
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                );

                if (!buffer) {
                        return std::unexpected(buffer.error());
                }

                ub = std::move(*buffer);

                if (auto result = ub.upload(UBO); !result) {
                        return std::unexpected(result.error());
                }
        }

        VkDescriptorPoolSize poolSize{
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 2
        };

        VkDescriptorPoolCreateInfo poolInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = 2,
                .poolSizeCount = 1,
                .pPoolSizes = &poolSize
        };

        VK_CHECK(
                vkCreateDescriptorPool(
                        device.getLogicalDevice(),
                        &poolInfo,
                        nullptr,
                        &uniformBuffer.descriptorPool
                ),
                LSIM::ErrorCode::VK_CREATE_DESCRIPTOR_POOL_FAILURE
        );

        std::array layouts{
                uniformBuffer.descriptorSetLayout,
                uniformBuffer.descriptorSetLayout
        };

        VkDescriptorSetAllocateInfo allocateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = uniformBuffer.descriptorPool,
                .descriptorSetCount = 2,
                .pSetLayouts = layouts.data()
        };

        VK_CHECK(
                vkAllocateDescriptorSets(
                        device.getLogicalDevice(),
                        &allocateInfo,
                        uniformBuffer.descriptorSets.data()
                ),
                LSIM::ErrorCode::VK_ALLOCATE_DESCRIPTOR_SETS_FAILURE
        );

        for (size_t i = 0; i < 2; ++i) {
                VkDescriptorBufferInfo bufferInfo{
                        .buffer = uniformBuffer.buffers[i].getBuffer(),
                        .offset = 0,
                        .range = size
                };

                VkWriteDescriptorSet write{
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = uniformBuffer.descriptorSets[i],
                        .dstBinding = 0,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .pBufferInfo = &bufferInfo
                };

                vkUpdateDescriptorSets(
                        device.getLogicalDevice(),
                        1,
                        &write,
                        0,
                        nullptr
                );
        }

        return uniformBuffer;
}

void UniformBuffer::destroy() {
        buffers = {};

        descriptorSets.fill(VK_NULL_HANDLE);

        if (descriptorPool != VK_NULL_HANDLE) {
                vkDestroyDescriptorPool(
                        device,
                        descriptorPool,
                        nullptr
                );
                descriptorPool = VK_NULL_HANDLE;
        }

        if (descriptorSetLayout != VK_NULL_HANDLE) {
                vkDestroyDescriptorSetLayout(
                        device,
                        descriptorSetLayout,
                        nullptr
                );
                descriptorSetLayout = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

UniformBuffer::~UniformBuffer() {
        destroy();
}
