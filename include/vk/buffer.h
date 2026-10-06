#ifndef LSIM_BUFFER_H
#define LSIM_BUFFER_H
#include <expected>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "LSIMtypes.h"

class Command;
class Buffer {
private:
        VkDeviceSize size{};
        VkDevice device{VK_NULL_HANDLE};
        VkBuffer buffer{VK_NULL_HANDLE};
        VkDeviceMemory deviceMemory{VK_NULL_HANDLE};

public:
        Buffer() = default;

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept;

        std::expected<void, LSIM::Error> upload(const void *data);

        Buffer& operator=(Buffer&& other) noexcept;

        static std::expected<Buffer, LSIM::Error> createFromData(const Device &device, Command &command, const void *data,
                                                          VkDeviceSize size, VkBufferUsageFlags usage);

        static std::expected<Buffer, LSIM::Error> create(const Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);


        [[nodiscard]] VkBuffer getBuffer() const { return buffer; }

        void destroy();

        ~Buffer();
};

#endif //LSIM_BUFFER_H
