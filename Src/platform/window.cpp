#include "platform/window.h"

#include "engineContext.h"
#include "GLFW/glfw3.h"
#include "utils/defaults.h"
#include "utils/logging/log.h"

std::optional<LSIM::Error> Window::initializeWindow(GLFWwindow*& window) {
        engineLogger("stdInfo", "starting L-SIMENGINE");

        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        const Defaults defaults = EngineContext::getDefaults();
        window = glfwCreateWindow(
                static_cast<int>(defaults.defaultWindowWidth),
                static_cast<int>(defaults.defaultWindowHeight),
                ("L-SIM ENGINE " + defaults.version + " " + EngineContext::getWorkingDir()).c_str(),
                nullptr,
                nullptr
        );

        if (window == nullptr) {
                engineLogger("stdError", "Failed to create GLFW window");
                glfwTerminate();
                return LSIM::Error{LSIM::ErrorCode::WINDOW_CREATION_FAILURE, LSIM::FatalityLevel::FATAL};
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        engineLogger("stdInfo", "Successfully created the GLFW window");

        return std::nullopt;
}
