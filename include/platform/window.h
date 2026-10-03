#ifndef LSIM_PLATFORM_H
#define LSIM_PLATFORM_H

#include <optional>
#include "LSIMtypes.h"
#include "utils/fileIO.h"

namespace Window {
        std::optional<LSIM::Error> initializeWindow(GLFWwindow *&window);
}

#endif //LSIM_PLATFORM_H
