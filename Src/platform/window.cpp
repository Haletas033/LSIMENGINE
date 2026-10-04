#include "platform/window.h"

#include <expected>

// #include "engineContext.h"
#include <iostream>

#include "utils/defaults.h"
// #include "utils/logging/log.h"

std::expected<void, LSIM::Error> Window::initializeWindow(GLFWwindow *&window) {
        // engineLogger("stdInfo", "starting L-SIMENGINE");
        std::cout << "starting L-SIM ENGINE\n";

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        const Defaults defaults{};
        window = glfwCreateWindow(
                static_cast<int>(defaults.defaultWindowWidth),
                static_cast<int>(defaults.defaultWindowHeight),
                ("L-SIM ENGINE " + defaults.version + " " + "TODO: Working dir").c_str(),
                nullptr,
                nullptr
        );

        if (window == nullptr) {
                // engineLogger("stdError", "Failed to create GLFW window");
                std::cout << "Failed to create GLFW window\n";
                glfwTerminate();
                return std::unexpected(
                        LSIM::Error{
                                LSIM::ErrorCode::WINDOW_CREATION_FAILURE,
                                LSIM::FatalityLevel::FATAL
                        }
                );
        }

        // engineLogger("stdInfo", "Successfully created the GLFW window");
        std::cout << "Successfully created the GLFW window\n";

        return{};
}
