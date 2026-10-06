#ifndef LSIM_UNIFORMBUFFER_H
#define LSIM_UNIFORMBUFFER_H
#include <array>
#include <expected>
#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "LSIMtypes.h"

class UniformBuffer {
private:
        VkDevice device{VK_NULL_HANDLE};
        std::array<Buffer, 2> buffers{};
        std::array<VkDescriptorSet, 2> descriptorSets{VK_NULL_HANDLE};
        VkDescriptorSetLayout descriptorSetLayout{VK_NULL_HANDLE};
        VkDescriptorPool descriptorPool{VK_NULL_HANDLE};

public:
        UniformBuffer() = default;

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;

        UniformBuffer(UniformBuffer&& other) noexcept;
        UniformBuffer& operator=(UniformBuffer&& other) noexcept;

        static std::expected<UniformBuffer, LSIM::Error> create(const Device &device, void *UBO, size_t size);

        [[nodiscard]] const std::array<Buffer, 2>& getBuffers() const { return buffers; }
        [[nodiscard]] const std::array<VkDescriptorSet, 2>& getDescriptorSets() const { return descriptorSets; }
        [[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

        void destroy();

        ~UniformBuffer();
};

#endif //LSIM_UNIFORMBUFFER_H
