#ifndef LSIM_COMMAND_H
#define LSIM_COMMAND_H
#include <array>
#include <expected>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "device.h"
#include "LSIMtypes.h"

class Command {
private:
        std::array<VkCommandBuffer, 2> buffers{VK_NULL_HANDLE};
        std::array<VkSemaphore, 2> imageAvailableSemaphores{VK_NULL_HANDLE};
        std::vector<VkSemaphore> renderFinishedSemaphores{};
        std::array<VkFence, 2> inFlightFences{VK_NULL_HANDLE};

        VkDevice device{VK_NULL_HANDLE};
        VkCommandPool pool{VK_NULL_HANDLE};

public:
        Command() = default;

        Command(const Command&) = delete;
        Command& operator=(const Command&) = delete;

        Command(Command&& other) noexcept;
        Command& operator=(Command&& other) noexcept;

        static std::expected<Command, LSIM::Error> create(const Device &device, uint32_t swapchainImageCount);

        [[nodiscard]] std::array<VkCommandBuffer, 2> getBuffers() const { return buffers; }
        [[nodiscard]] std::array<VkSemaphore, 2> getImageAvailableSemaphores() const {
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
