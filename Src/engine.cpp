#include "include/engine.h"

#include "ECS/name.h"
#include "glad/glad.h"
#include "rendering/light.h"
#include "utils/fileIO.h"
#include "utils/json.h"
#include "utils/texture.h"

using json = nlohmann::ordered_json;

void Engine::framebufferSizeCallback(GLFWwindow *w, int width, int height) {
        glViewport(0, 0, width, height);
}

Engine::Engine(const int argc, char **argv) {
        if (argc >= 2) {
                for (int i = 1; i < argc; ++i) {
                        workingDir += argv[i];
                        if (i != argc - 1)
                                workingDir += ' ';
                }
                workingDir += '/';
        }
}

std::optional<LSIMErrorCode> Engine::start() {
        // Load config
        engineDefaults = JSONManager::InitJSON(workingDir + "config/config.json", config);
        Logger::InitEngineLogger();

        // Load shaders
        std::string vertexShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.vert", config);
        std::string fragmentShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.frag", config);

        std::string skyboxVert = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.vert", config);
        std::string skyboxFrag = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.frag", config);

        IO::InitIO();
        Texture::InitTextures();

        engineLogger("stdInfo", "starting L-SIMENGINE");

        // Create window
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(
                static_cast<int>(engineDefaults.defaultWindowWidth),
                static_cast<int>(engineDefaults.defaultWindowHeight),
                ("L-SIM ENGINE " + engineDefaults.version + " " + workingDir).c_str(),
                nullptr,
                nullptr
        );

        // Error check if the window fails to create
        if (window == nullptr) {
                engineLogger("stdError", "Failed to create window");
                glfwTerminate();
                return LSIMErrorCode::WINDOW_CREATION_FAILURE;
        }

        engineLogger("stdInfo", "Successfully created the window");

        // Introduce the window into the current context and enable v-sync
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        gladLoadGL();
        glViewport(
                0,
                0,
                static_cast<int>(engineDefaults.defaultWindowWidth),
                static_cast<int>(engineDefaults.defaultWindowHeight)
        );
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        // Create shaders
        Shader shaderProgram({vertexShader, fragmentShader});
        ResourceManager::addShader("PBRShader", std::move(shaderProgram));

        Shader skyboxShaderProgram({skyboxVert, skyboxFrag});
        ResourceManager::addShader("skyboxShader", std::move(skyboxShaderProgram));

        Gui::Initialize(window);

        // Create basic scene
        if (!workingDir.empty()) {
                EntityHandle firstLight = Light::create(Registry::getDefaultRegistry(), Light::Type::POINT);
                Registry::getDefaultRegistry().getComponent<Name>(firstLight)->value = "First Light";

                EntityHandle firstCube = Mesh::create(Primitive::CUBE, MeshMode::STATIC, Registry::getDefaultRegistry(), MeshPool::getDefaultMeshPool(), Material::createStandardPBR(), Transform());
                Registry::getDefaultRegistry().getComponent<Name>(firstCube)->value = "First Cube";

                sharedState.current_entities() = {firstCube};
        }



        return std::nullopt;
}
