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
        int windowWidth{}, windowHeight{};
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
        EntityHandle skybox = EntityHandle::invalid();

        [[nodiscard]] std::optional<LSIM::Error> initializeConfig();
        [[nodiscard]] std::optional<LSIM::Error> loadShaders();
        [[nodiscard]] std::optional<LSIM::Error> loadResources();
        [[nodiscard]] std::optional<LSIM::Error> initializeSystems();
        [[nodiscard]] static std::optional<LSIM::Error> initializeScripts();
        [[nodiscard]] std::optional<LSIM::Error> updateSystems();

        void updateAspect();

        void updateTime();

public:
        Engine(int argc, char **argv);

        [[nodiscard]] std::optional<LSIM::Error> start();
        [[nodiscard]] std::optional<LSIM::Error> update();

        ~Engine();
};

#endif //LSIM_ENGINE_H
