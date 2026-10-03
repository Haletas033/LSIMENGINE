#ifndef LSIM_GL_H
#define LSIM_GL_H
#include <optional>

#include "engineContext.h"
#include "LSIMtypes.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "utils/defaults.h"

namespace GL {
        inline void frameBufferSizeCallback(GLFWwindow* window, const int width, const int height){
                glViewport(0, 0, width, height);
        }

        std::optional<LSIM::Error> initializeOpenGL(GLFWwindow* window);
}

#endif //LSIM_GL_H
