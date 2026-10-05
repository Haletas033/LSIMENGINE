#include "vk/command.h"

#include "vk/device.h"
#include "vk/vk.h"

Command::Command(Command &&other) noexcept
        : buffers(other.buffers),
          imageAvailableSemaphores(other.imageAvailableSemaphores),
          renderFinishedSemaphores(other.renderFinishedSemaphores),
          inFlightFences(other.inFlightFences),
          device(other.device),
          pool(other.pool)
{
        other.buffers.fill(VK_NULL_HANDLE);
        other.imageAvailableSemaphores.fill(VK_NULL_HANDLE);
        other.renderFinishedSemaphores.clear();
        other.inFlightFences.fill(VK_NULL_HANDLE);
        other.device = VK_NULL_HANDLE;
        other.pool = VK_NULL_HANDLE;
}

Command &Command::operator=(Command &&other) noexcept {
        if (this == &other)
                return *this;

        destroy();

        buffers = other.buffers;
        imageAvailableSemaphores = other.imageAvailableSemaphores;
        renderFinishedSemaphores = other.renderFinishedSemaphores;
        inFlightFences = other.inFlightFences;
        device = other.device;
        pool = other.pool;

        other.buffers.fill(VK_NULL_HANDLE);
        other.imageAvailableSemaphores.fill(VK_NULL_HANDLE);
        other.renderFinishedSemaphores.clear();
        other.inFlightFences.fill(VK_NULL_HANDLE);
        other.device = VK_NULL_HANDLE;
        other.pool = VK_NULL_HANDLE;

        return *this;
}

std::expected<Command, LSIM::Error> Command::create(const Device& device, const uint32_t swapchainImageCount) {
        Command command{};
        command.device = device.getLogicalDevice();

        const VkCommandPoolCreateInfo commandPoolCreateInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = device.getGraphicsQueueFamily(),
        };
        VK_CHECK(
                vkCreateCommandPool(
                        command.device,
                        &commandPoolCreateInfo,
                        nullptr,
                        &command.pool
                ),
                LSIM::ErrorCode::VK_CREATE_COMMAND_POOL_FAILURE
        );

        const VkCommandBufferAllocateInfo commandBufferAllocateInfo{
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = command.pool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 2
        };
        VK_CHECK(
                vkAllocateCommandBuffers(
                        command.device,
                        &commandBufferAllocateInfo,
                        command.buffers.data()
                ),
                LSIM::ErrorCode::VK_ALLOCATE_COMMAND_BUFFERS_FAILURE
        );

        constexpr VkSemaphoreCreateInfo semaphoreCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        for (uint32_t i{}; i < command.buffers.size(); ++i) {


                VK_CHECK(
                        vkCreateSemaphore(
                                command.device,
                                &semaphoreCreateInfo,
                                nullptr,
                                &command.imageAvailableSemaphores[i]
                        ),
                        LSIM::ErrorCode::VK_CREATE_SEMAPHORE_FAILURE
                );

                VkFenceCreateInfo fenceCreateInfo{
                        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                        .flags = VK_FENCE_CREATE_SIGNALED_BIT
                };

                VK_CHECK(
                        vkCreateFence(
                                command.device,
                                &fenceCreateInfo,
                                nullptr,
                                &command.inFlightFences[i]
                        ),
                        LSIM::ErrorCode::VK_CREATE_FENCE_FAILURE
                );
        }

        for (uint32_t i{}; i < swapchainImageCount; ++i) {
                VkSemaphore renderFinishedSemaphore{VK_NULL_HANDLE};
                VK_CHECK(
                        vkCreateSemaphore(
                                command.device,
                                &semaphoreCreateInfo,
                                nullptr,
                                &renderFinishedSemaphore
                        ),
                        LSIM::ErrorCode::VK_CREATE_SEMAPHORE_FAILURE
                );
                command.renderFinishedSemaphores.push_back(renderFinishedSemaphore);
        }

        return command;
}

void Command::destroy() {
        for (auto& semaphore : imageAvailableSemaphores) {
                if (semaphore != VK_NULL_HANDLE) {
                        vkDestroySemaphore(device, semaphore, nullptr);
                        semaphore = VK_NULL_HANDLE;
                }
        }

        for (auto& semaphore : renderFinishedSemaphores) {
                if (semaphore != VK_NULL_HANDLE) {
                        vkDestroySemaphore(device, semaphore, nullptr);
                        semaphore = VK_NULL_HANDLE;
                }
        }

        for (auto& fence : inFlightFences) {
                if (fence != VK_NULL_HANDLE) {
                        vkDestroyFence(device, fence, nullptr);
                        fence = VK_NULL_HANDLE;
                }
        }

        if (pool != VK_NULL_HANDLE) {
                vkDestroyCommandPool(device, pool, nullptr);
                pool = VK_NULL_HANDLE;
        }

        buffers.fill(VK_NULL_HANDLE);
        device = VK_NULL_HANDLE;
}

Command::~Command() {
        destroy();
}
