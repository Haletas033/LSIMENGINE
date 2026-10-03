#ifndef LSIM_ENGINE_H
#define LSIM_ENGINE_H
#include "editor/editorInputs.h"
#include "editor/sharedState.h"
#include "nlohmann/json.hpp"
#include "scene/camera.h"
#include "scene/scene.h"
#include "utils/defaults.h"

class Engine {
private:
        double mouseX{}, mouseY{};
        float aspect{};
        float deltaTime{};
        float lastTime{};
        Defaults engineDefaults{};
        nlohmann::ordered_json config{};
        Scene scene {};
        Camera camera {};
        GLFWwindow* window = nullptr;
        std::string workingDir;
        SharedState sharedState{};
        Inputs inputs{};
        EditorInputs editorInputs{};

        static void frameBufferSizeCallback(GLFWwindow* window, const int width, const int height){
                glViewport(0, 0, width, height);
        }

public:
        Engine(int argc, char **argv);

        void start();

        void update();

        void exit();
};

#endif //LSIM_ENGINE_H
