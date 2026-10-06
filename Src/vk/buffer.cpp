#include "vk/buffer.h"

#include <cstring>
#include <expected>

#include "LSIMtypes.h"
#include "vk/command.h"
#include "vk/device.h"
#include "vk/vk.h"

class Command;

Buffer::Buffer(Buffer &&other) noexcept
        : size(other.size),
          device(other.device),
          buffer(other.buffer),
          deviceMemory(other.deviceMemory)
{
        other.size = {};
        other.device = VK_NULL_HANDLE;
        other.buffer = VK_NULL_HANDLE;
        other.deviceMemory = VK_NULL_HANDLE;
}

std::expected<void, LSIM::Error> Buffer::upload(const void* data) const {
        void* mapped = nullptr;

        VK_CHECK(
                vkMapMemory(
                        device,
                        deviceMemory,
                        0,
                        size,
                        0,
                        &mapped
                ),
                LSIM::ErrorCode::VK_MEMORY_MAP_FAILURE
        );

        memcpy(mapped, data, size);

        vkUnmapMemory(device, deviceMemory);

        return {};
}

Buffer &Buffer::operator=(Buffer &&other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        size = other.size;
        device = other.device;
        buffer = other.buffer;
        deviceMemory = other.deviceMemory;

        other.size = {};
        other.device = VK_NULL_HANDLE;
        other.buffer = VK_NULL_HANDLE;
        other.deviceMemory = VK_NULL_HANDLE;

        return *this;
}

std::expected<Buffer, LSIM::Error> Buffer::createFromData(
        const Device& device,
        Command& command,
        const void* data,
        const VkDeviceSize size,
        const VkBufferUsageFlags usage
) {
        auto stagingBuffer = create(
        device,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
);
        if (!stagingBuffer)
                return std::unexpected(stagingBuffer.error());

        if (auto result = stagingBuffer->upload(data); !result) {
                return std::unexpected(result.error());
        }

        auto buffer = create(
                device,
                size,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
        if (!buffer)
                return std::unexpected(buffer.error());

        auto copyBuffer = command.copyBuffer(
                stagingBuffer->getBuffer(),
                buffer->getBuffer(),
                size,
                device.getGraphicsQueue()
        );

        if (!copyBuffer) { return std::unexpected(copyBuffer.error()); }

        return buffer;
}

std::expected<Buffer, LSIM::Error> Buffer::create(
        const Device& device,
        const VkDeviceSize size,
        const VkBufferUsageFlags usage,
        const VkMemoryPropertyFlags memoryProperties
) {
        Buffer buffer{};
        buffer.size = size;
        buffer.device = device.getLogicalDevice();

        const VkBufferCreateInfo bufferCreateInfo {
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = buffer.size,
                .usage = usage,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VK_CHECK(
                vkCreateBuffer(
                        buffer.device,
                        &bufferCreateInfo,
                        nullptr,
                        &buffer.buffer
                ),
                LSIM::ErrorCode::VK_CREATE_BUFFER_FAILURE
        );

        VkMemoryRequirements memoryRequirements{};
        vkGetBufferMemoryRequirements(buffer.device, buffer.buffer, &memoryRequirements);

        auto memoryType = device.getMemoryType(
                memoryRequirements,
                memoryProperties
        );

        if (!memoryType) { return std::unexpected(memoryType.error()); }

        const VkMemoryAllocateInfo memoryAllocateInfo {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memoryRequirements.size,
                .memoryTypeIndex = *memoryType
        };

        VK_CHECK(
                vkAllocateMemory(
                        buffer.device,
                        &memoryAllocateInfo,
                        nullptr,
                        &buffer.deviceMemory
                ),
                LSIM::ErrorCode::VK_MEMORY_ALLOCATION_FAILURE
        );

        VK_CHECK(
                vkBindBufferMemory(
                        buffer.device,
                        buffer.buffer,
                        buffer.deviceMemory,
                        0
                ),
                LSIM::ErrorCode::VK_BIND_BUFFER_FAILURE
        );

        return buffer;
}

void Buffer::destroy() {
        size = {};

        if (buffer != VK_NULL_HANDLE) {
                vkDestroyBuffer(
                        device,
                        buffer,
                        nullptr
                );
                buffer = VK_NULL_HANDLE;
        }

        if (deviceMemory != VK_NULL_HANDLE) {
                vkFreeMemory(
                        device,
                        deviceMemory,
                        nullptr
                );

                deviceMemory = VK_NULL_HANDLE;
        }

        device = VK_NULL_HANDLE;
}

Buffer::~Buffer() {
        destroy();
}
