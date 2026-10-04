#ifndef LSIM_VK_H
#define LSIM_VK_H

#include <optional>
#include <vulkan/vulkan_core.h>
#include <expected>
#include <iostream>

#include "LSIMtypes.h"

namespace VK {
        inline std::optional<LSIM::Error> check(const VkResult result, LSIM::ErrorCode errorCode) {
                if (result != VK_SUCCESS) {
                        return LSIM::Error{
                                errorCode,
                                LSIM::FatalityLevel::FATAL
                        };
                }

                return std::nullopt;
        }
}

#define VK_CHECK(result, errCode) do {\
        if (const auto err = VK::check((result), (errCode)); err) {\
                std::cout << "FATAL: " << #errCode << '\n';\
                return std::unexpected(*err);\
        }\
} while (0)

#endif //LSIM_VK_H
