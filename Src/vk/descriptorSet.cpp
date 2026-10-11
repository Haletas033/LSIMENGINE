#include "vk/descriptorSet.h"

#include <vector>

#include "vk/vk.h"

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
        : device(other.device),
          layout(other.layout),
          pool(other.pool),
          sets(other.sets)
{
        other.device = VK_NULL_HANDLE;
        other.layout = VK_NULL_HANDLE;
        other.pool = VK_NULL_HANDLE;
        other.sets.fill(VK_NULL_HANDLE);
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        device = other.device;
        layout = other.layout;
        pool = other.pool;
        sets = other.sets;

        other.device = VK_NULL_HANDLE;
        other.layout = VK_NULL_HANDLE;
        other.pool = VK_NULL_HANDLE;
        other.sets.fill(VK_NULL_HANDLE);

        return *this;
}

std::expected<DescriptorSet, LSIM::Error> DescriptorSet::create(
        VkDevice device,
        std::span<const BindingDescriptor> bindings,
        std::array<std::span<const WriteDescriptor>, 2> writes
) {
        DescriptorSet descriptorSet{};
        descriptorSet.device = device;

        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        for (const auto& [binding, type, stage] : bindings) {
                layoutBindings.push_back({
                    .binding = binding,
                    .descriptorType = type,
                    .descriptorCount = 1,
                    .stageFlags = stage
                });
        }

        const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
                .pBindings = layoutBindings.data()
        };

        VK_CHECK(
                vkCreateDescriptorSetLayout(
                        device,
                        &descriptorSetLayoutCreateInfo,
                        nullptr,
                        &descriptorSet.layout
                ),
                LSIM::ErrorCode::VK_CREATE_DESCRIPTOR_SET_LAYOUT_FAILURE
        );

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (const auto& [binding, type, stage] : bindings) {
                poolSizes.push_back({
                    .type = type,
                    .descriptorCount = 2
                });
        }

        const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = 2,
                .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
                .pPoolSizes = poolSizes.data()
        };

        VK_CHECK(
                vkCreateDescriptorPool(
                        device,
                        &descriptorPoolCreateInfo,
                        nullptr,
                        &descriptorSet.pool
                ),
                LSIM::ErrorCode::VK_CREATE_DESCRIPTOR_POOL_FAILURE
        );

        std::array setLayouts{descriptorSet.layout, descriptorSet.layout};
        const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = descriptorSet.pool,
                .descriptorSetCount = 2,
                .pSetLayouts = setLayouts.data()
        };

        VK_CHECK(
                vkAllocateDescriptorSets(
                        device,
                        &descriptorSetAllocateInfo,
                        descriptorSet.sets.data()
                ),
                LSIM::ErrorCode::VK_ALLOCATE_DESCRIPTOR_SETS_FAILURE
        );

        for (uint32_t frame = 0; frame < 2; ++frame) {
                std::vector<VkWriteDescriptorSet> writeSets;
                for (const auto& write : writes[frame]) {
                        VkWriteDescriptorSet writeSet{
                                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                .dstSet = descriptorSet.sets[frame],
                                .dstBinding = write.binding,
                                .descriptorCount = 1,
                                .descriptorType = write.type,
                        };

                        std::visit([&]<typename T0>(const T0& info) {
                                using T = std::decay_t<T0>;
                                if constexpr (std::is_same_v<T, VkDescriptorBufferInfo>)
                                        writeSet.pBufferInfo = &info;
                                else
                                        writeSet.pImageInfo = &info;
                        }, write.info);
                        writeSets.push_back(writeSet);
                }
                vkUpdateDescriptorSets(device, writeSets.size(), writeSets.data(), 0, nullptr);
        }

        return descriptorSet;
}

void DescriptorSet::destroy() {
        if (pool != VK_NULL_HANDLE) {
                vkDestroyDescriptorPool(device, pool, nullptr);
                pool = VK_NULL_HANDLE;
        }

        if (layout != VK_NULL_HANDLE) {
                vkDestroyDescriptorSetLayout(device, layout, nullptr);
                layout = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

DescriptorSet::~DescriptorSet() {
        destroy();
}
