#ifndef LSIM_ENGINE_H
#define LSIM_ENGINE_H

#include <string>

#include "editor/editorInputs.h"
#include "editor/sharedState.h"
#include "inputs/inputs.h"
#include "nlohmann/json.hpp"
#include "scene/camera.h"
#include "scene/scene.h"
#include "utils/defaults.h"

enum class LSIMErrorCode {
        WINDOW_CREATION_FAILURE
};

class Engine {
private:
        nlohmann::ordered_json config;
        Defaults engineDefaults{};
        Scene scene{};
        Camera camera{};
        GLFWwindow* window = nullptr;
        std::string workingDir{};
        SharedState sharedState{};
        Inputs inputs{};
        EditorInputs editorInputs{};

        static void framebufferSizeCallback(GLFWwindow* w, int width, int height);

        Engine(int argc, char** argv);

        std::optional<LSIMErrorCode> start();
};

#endif //LSIM_ENGINE_H
