#include "gl/gl.h"

std::optional<LSIM::Error> GL::initializeOpenGL(GLFWwindow* window) {
        if (!gladLoadGL(glfwGetProcAddress)) {
                return LSIM::Error{LSIM::ErrorCode::GLAD_LOAD_FAILURE, LSIM::FatalityLevel::FATAL};
        }

        glViewport(
                0, 0,
                static_cast<int>(EngineContext::getDefaults().defaultWindowWidth),
                static_cast<int>(EngineContext::getDefaults().defaultWindowHeight)
        );

        glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

        glEnable(GL_DEPTH_TEST);

        return std::nullopt;
}
