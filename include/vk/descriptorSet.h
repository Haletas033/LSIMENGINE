#ifndef LSIM_DESCRIPTORSET_H
#define LSIM_DESCRIPTORSET_H
#include <array>
#include <cstdint>
#include <expected>
#include <span>
#include <variant>
#include <vulkan/vulkan_core.h>

#include "LSIMtypes.h"

class DescriptorSet {
public:
        struct BindingDescriptor {
                uint32_t binding;
                VkDescriptorType type;
                VkShaderStageFlags stage;
        };

        struct WriteDescriptor {
                uint32_t binding;
                VkDescriptorType type;
                std::variant<VkDescriptorBufferInfo, VkDescriptorImageInfo> info;
        };

private:
        VkDevice device{VK_NULL_HANDLE};
        VkDescriptorSetLayout layout{VK_NULL_HANDLE};
        VkDescriptorPool pool{VK_NULL_HANDLE};
        std::array<VkDescriptorSet, 2> sets{};

public:
        DescriptorSet() = default;

        DescriptorSet(const DescriptorSet&) = delete;
        DescriptorSet& operator=(const DescriptorSet&) = delete;

        DescriptorSet(DescriptorSet&& other) noexcept;
        DescriptorSet& operator=(DescriptorSet&& other) noexcept;

        static std::expected<DescriptorSet, LSIM::Error> create(
                VkDevice device,
                std::span<const BindingDescriptor> bindings,
                std::array<std::span<const WriteDescriptor>, 2> writes
        );

        [[nodiscard]] VkDescriptorSetLayout getLayout() const { return layout; }
        [[nodiscard]] std::array<VkDescriptorSet, 2> getSets() const { return sets; }

        void destroy();

        ~DescriptorSet();
};

#endif //LSIM_DESCRIPTORSET_H
