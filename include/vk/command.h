#ifndef LSIM_COMMAND_H
#define LSIM_COMMAND_H
#include <array>
#include <expected>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "LSIMtypes.h"

#define FRAMES_IN_FLIGHT 2

class Command {
private:
        std::vector<VkCommandBuffer> buffers{};
        std::array<VkSemaphore, FRAMES_IN_FLIGHT> imageAvailableSemaphores{VK_NULL_HANDLE};
        std::vector<VkSemaphore> renderFinishedSemaphores{};
        std::array<VkFence, FRAMES_IN_FLIGHT> inFlightFences{VK_NULL_HANDLE};

        VkDevice device{VK_NULL_HANDLE};
        VkCommandPool pool{VK_NULL_HANDLE};

public:
        Command() = default;

        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;

        Command(Command&& other) noexcept;
        Command& operator=(Command&& other) noexcept;

        std::expected<void, LSIM::Error> copyBuffer(
                VkBuffer source,
                VkBuffer destination,
                VkDeviceSize size, VkQueue graphicsQueue
        );

        static std::expected<Command, LSIM::Error> create(const Device &device, uint32_t swapchainImageCount);

        [[nodiscard]] std::vector<VkCommandBuffer> getBuffers() const { return buffers; }
        [[nodiscard]] std::array<VkSemaphore, FRAMES_IN_FLIGHT> getImageAvailableSemaphores() const {
                return imageAvailableSemaphores;
        }
        [[nodiscard]] std::vector<VkSemaphore> getRenderFinishedSemaphores() const {
                return renderFinishedSemaphores;
        }
        [[nodiscard]] std::array<VkFence, 2> getInFlightFences() const { return inFlightFences; }
        [[nodiscard]] VkCommandPool getPool() const { return pool; }

        void destroy();

        ~Command();
};

#endif //LSIM_COMMAND_H
