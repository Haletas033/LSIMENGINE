#ifndef LSIM_PLATFORM_H
#define LSIM_PLATFORM_H

#include <expected>
#include <optional>
#include "LSIMtypes.h"
#include "GLFW/glfw3.h"
// #include "utils/fileIO.h"

namespace Window {
        std::expected<void, LSIM::Error> initializeWindow(GLFWwindow *&window);
}

#endif //LSIM_PLATFORM_H
