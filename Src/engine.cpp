#include "engine.h"

#include "ECS/name.h"
#include "ECS/nameSystem.h"
#include "ECS/systemManager.h"
#include "geometry/transformSystem.h"
#include "rendering/light.h"
#include "rendering/renderSystem.h"
#include "scene/script.h"
#include "utils/fileIO.h"
#include "utils/json.h"
#include "utils/meshPicking.h"
#include "utils/texture.h"

static Defaults s_defaults;
static std::string s_working;
static nlohmann::ordered_json s_config;

Engine::Engine(int argc, char** argv) {
	if (argc >= 2) {
		for (int i = 1; i < argc; ++i) {
			workingDir += argv[i];
			if (i != argc - 1)
				workingDir += ' ';
		}
		workingDir += '/';
	}
}

void Engine::start() {
	//Load config
	engineDefaults = JSONManager::InitJSON(workingDir + "config/config.json", config);

	s_defaults = engineDefaults;
	s_working = workingDir;
	s_config = config;

	Logger::InitEngineLogger();

	//Load shaders

	//default shaders
	std::string vertexShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.vert", config);
	std::string fragmentShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.frag", config);

	std::string skyboxVert = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.vert", config);
	std::string skyboxFrag = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.frag", config);

	IO::InitIO();
	Texture::InitTextures();

	engineLogger("stdInfo", "starting L-SIMENGINE");

	//Initialize GLFW
	glfwInit();

	//Tell GLFW what version of OpenGL we are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//Tell GLFW we are using the CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create a GLFW window object of 800 by 800 pixels
	window = glfwCreateWindow(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, ("L-SIM ENGINE " + engineDefaults.version + " " + workingDir).c_str(), nullptr, nullptr);

	//Error check if the window fails to create
	if (window == nullptr) {
		engineLogger("stdError", "Failed to create GLFW window");
		glfwTerminate();
	}

	inputs.InitInputs(window);
	editorInputs.Init(Registry::getDefaultRegistry(), MeshPool::getDefaultMeshPool(), scene, workingDir, sharedState, engineDefaults, camera, inputs);

	Script::InstantiateAll();

	engineLogger("stdInfo", "Successfully created the GLFW window");

	//Introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	//Specify the viewport of OpenGL in the Window
	glViewport(0, 0, engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight);

	// Register the window resize callback function
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	Shader shaderProgram({vertexShader, fragmentShader});
	ResourceManager::addShader("PBRShader", std::move(shaderProgram));


	Shader skyboxShaderProgram({skyboxVert, skyboxFrag});
	ResourceManager::addShader("skyboxShader", std::move(skyboxShaderProgram));

	Gui::Initialize(window);

	EntityHandle firstLight = Light::create(Registry::getDefaultRegistry(), Light::Type::POINT);
	Registry::getDefaultRegistry().getComponent<Name>(firstLight)->value = "First Light";

	EntityHandle firstCube = Mesh::create(Primitive::CUBE, MeshMode::STATIC, Registry::getDefaultRegistry(), MeshPool::getDefaultMeshPool(), Material::createStandardPBR(), Transform());
	sharedState.current_entities() = {firstCube};
	Registry::getDefaultRegistry().getComponent<Name>(firstCube)->value = "First Cube";

	//Skybox faces
	std::array<std::string, 6> faces = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg",
	};

	//Get skybox texture id
	GLuint skyboxTexId = Texture::GetCubemapId(faces, GL_LINEAR);

	Material skyboxMat;
	skyboxMat.addShader("skyboxShader");
	skyboxMat.addProperty("skybox", static_cast<int>(skyboxTexId));
	EntityHandle skybox = Mesh::create(
	    Primitive::CUBE,
	    MeshMode::STATIC,
	    Registry::getDefaultRegistry(),
	    MeshPool::getDefaultMeshPool(),
	    skyboxMat
	);
	Registry::getDefaultRegistry().getComponent<Name>(skybox)->value = "Skybox";

	engineLogger("stdInfo", "Successfully created the default \"First Cube\"");

	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	camera = Camera(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	engineLogger("stdInfo", "Successfully created the camera object");

	engineLogger("stdInfo", "Successfully moved meshes and lights into the main scene");

	if (!workingDir.empty()) {
		for (const auto &file : std::filesystem::recursive_directory_iterator(workingDir)) {
			if (file.path().extension().string() == ".lsim") {
				engineLogger("stdInfo", file.path().string());
				std::ifstream LSIMfile(file.path().string(), std::ios::binary);
				IO::loadFromFile(LSIMfile, Registry::getDefaultRegistry(), sharedState, workingDir);
			}
		}
	}

	SystemManager::getDefaultSystemManager().addAtStage<NameSystem>(SystemStage::PRE_UPDATE);
	SystemManager::getDefaultSystemManager().addAtStage<TransformSystem>(SystemStage::PRE_PHYSICS);

	SystemManager::getDefaultSystemManager().addAtStage<RenderSystem>(
		std::make_unique<RenderSystem>(MeshPool::getDefaultMeshPool(), camera, scene, aspect),
		SystemStage::RENDER
	);

	//Run Start() for all scripts
	for (auto script : Script::GetAllScripts()) script->Start();
}

void Engine::update() {
	//Main render loop
	engineLogger("stdInfo", "Starting main gameplay loop");
	while (!glfwWindowShouldClose(window)) {
		//Update aspect ratio from current framebuffer size
		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

		//Check if the window is minimized if so skip render loop and just poll events
		if (windowWidth <= 0 || windowHeight <= 0) {
			glfwPollEvents();
			continue;
		}

		auto currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		//Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handle camera inputs

		// Only process camera movement if ImGui is not using the mouse

		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
			#ifndef GAME
			camera.Inputs(window, deltaTime);
			#endif
			inputs.handleInputs((Inputs::InputContext){scene, deltaTime});inputs.handleInputs((Inputs::InputContext){scene, deltaTime});
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			auto viewport = glm::vec4(0.0f, 0.0f, windowWidth, windowHeight);
			auto rayDir = meshPicking::GetMouseRay(mouseX, mouseY, camera.projection, camera.view, viewport);
			if (const auto e = meshPicking::pickMesh(Registry::getDefaultRegistry(), camera.Position, rayDir); e.has_value()) {
				sharedState.current_entities() = {e.value()};
			}
		}

		SystemManager::getDefaultSystemManager().update(Registry::getDefaultRegistry(), deltaTime);

		//Run Update() function for all scripts
		for (auto script : Script::GetAllScripts()) script->Update(deltaTime);

		#ifndef GAME
		Gui::Begin();

		ImVec2 displaySize = ImGui::GetIO().DisplaySize; // (width, height)

		ImVec2 mainUISize(displaySize.x * 0.2f, displaySize.y);
		ImVec2 mainUIPos(displaySize.x - mainUISize.x, 0);

		ImGui::SetNextWindowPos(mainUIPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(mainUISize, ImGuiCond_Always);

		ImGui::Begin("Main UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Gui::Main(Registry::getDefaultRegistry(), sharedState);
		// Gui::SceneGUI(skybox, scene.ambientLightColour, scene.ambientLightIntensity);

		Gui::Debug(mouseX, mouseY);

		ImGui::End();

		ImVec2 hierarchySize(displaySize.x * 0.2f, displaySize.y);
		ImVec2 hierarchyPos(0, 0);

		ImGui::SetNextWindowPos(hierarchyPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(hierarchySize, ImGuiCond_Always);

		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (std::optional<EntityHandle> clickedMesh = Gui::Hierarchy(Registry::getDefaultRegistry()); clickedMesh) sharedState.current_entities() = {clickedMesh.value()};

		ImGui::End();

		ImVec2 consoleSize(displaySize.x * 0.6f, displaySize.y * 0.3f);
		ImVec2 consolePos(displaySize.x * 0.2f, displaySize.y * 0.7f);

		ImGui::SetNextWindowPos(consolePos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(consoleSize, ImGuiCond_Always);

		ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Gui::Console();

		ImGui::End();

		Gui::End();
		#endif

		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}
}

void Engine::exit() {
	engineLogger("stdInfo", "Exiting L-SIMENGINE");

	Gui::DeleteNodeRecursively(Gui::root);
	Gui::CleanUp();

	Registry::getDefaultRegistry() = Registry{};
	MeshPool::getDefaultMeshPool() = MeshPool{};
	ResourceManager::clear();

	glfwDestroyWindow(window);
	glfwTerminate();

	engineLogger("stdInfo", "Successfully exited L-SIMENGINE");
}

Defaults& EngineContext::getDefaults() {
	return s_defaults;
}

const nlohmann::ordered_json& EngineContext::getConfig() {
	return s_config;
}

const std::string& EngineContext::getWorkingDir() {
	return s_working;
}