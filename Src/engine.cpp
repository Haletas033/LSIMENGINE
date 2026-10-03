#include "engine.h"

#include "ECS/name.h"
#include "ECS/nameSystem.h"
#include "ECS/systemManager.h"
#include "editor/editorGUI.h"
#include "geometry/transformSystem.h"
#include "gl/gl.h"
#include "platform/window.h"
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

Engine::Engine(const int argc, char** argv) {
	if (argc >= 2) {
		for (int i = 1; i < argc; ++i) {
			workingDir += argv[i];
			if (i != argc - 1)
				workingDir += ' ';
		}
		workingDir += '/';
	}
}

std::optional<LSIM::Error> Engine::initializeConfig() {
	//Load config
	engineDefaults = JSONManager::InitJSON(workingDir + "config/config.json", config);

	s_defaults = engineDefaults;
	s_working = workingDir;
	s_config = config;

	Logger::InitEngineLogger();

	return std::nullopt;
}

std::optional<LSIM::Error> Engine::loadShaders() {
	std::string vertexShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.vert", config);
	std::string fragmentShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.frag", config);

	std::string skyboxVert = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.vert", config);
	std::string skyboxFrag = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.frag", config);

	auto [PBRSdr, PBRErr] = Shader::Create({vertexShader, fragmentShader});
	if (PBRErr) { return PBRErr; }
	ResourceManager::addShader("PBRShader", std::move(PBRSdr));

	auto [skySdr, skyErr] = Shader::Create({skyboxVert, skyboxFrag});
	if (skyErr) { return skyErr; }
	ResourceManager::addShader("skyboxShader", std::move(skySdr));

	return std::nullopt;
}

std::optional<LSIM::Error> Engine::loadResources() {
	IO::InitIO();
	Gui::Initialize(window);
	Texture::InitTextures();

	return loadShaders();
}

std::optional<LSIM::Error> Engine::initializeSystems() {
	SystemManager::getDefaultSystemManager().addAtStage<NameSystem>(SystemStage::PRE_UPDATE);
	SystemManager::getDefaultSystemManager().addAtStage<TransformSystem>(SystemStage::PRE_PHYSICS);

	SystemManager::getDefaultSystemManager().addAtStage<RenderSystem>(
		std::make_unique<RenderSystem>(MeshPool::getDefaultMeshPool(), camera, scene, aspect),
		SystemStage::RENDER
	);

	return std::nullopt;
}

std::optional<LSIM::Error> Engine::initializeScripts() {
	Script::InstantiateAll();
	for (const auto script : Script::GetAllScripts()) script->Start();

	engineLogger("stdInfo", "Successfully started all scripts");

	return std::nullopt;
}

void Engine::updateAspect() {
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	if (windowHeight > 0) {
		aspect = static_cast<float>(windowWidth)
			/ static_cast<float>(windowHeight);
	}
}

void Engine::updateTime() {
	const auto currentTime = static_cast<float>(glfwGetTime());
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
}

std::optional<LSIM::Error> Engine::updateSystems() {
	SystemManager::getDefaultSystemManager().update(SystemStage::PRE_UPDATE, Registry::getDefaultRegistry(), deltaTime);

	SystemManager::getDefaultSystemManager().update(SystemStage::PRE_PHYSICS, Registry::getDefaultRegistry(), deltaTime);
	SystemManager::getDefaultSystemManager().update(SystemStage::PHYSICS, Registry::getDefaultRegistry(), deltaTime);
	SystemManager::getDefaultSystemManager().update(SystemStage::POST_PHYSICS, Registry::getDefaultRegistry(), deltaTime);

	if (windowWidth > 0 && windowHeight > 0) {
		SystemManager::getDefaultSystemManager().update(SystemStage::PRE_RENDER, Registry::getDefaultRegistry(), deltaTime);
		SystemManager::getDefaultSystemManager().update(SystemStage::RENDER, Registry::getDefaultRegistry(), deltaTime);
		SystemManager::getDefaultSystemManager().update(SystemStage::POST_RENDER, Registry::getDefaultRegistry(), deltaTime);
	}

	SystemManager::getDefaultSystemManager().update(SystemStage::POST_UPDATE, Registry::getDefaultRegistry(), deltaTime);

	return std::nullopt;
}

std::optional<LSIM::Error> Engine::start() {
	LSIM_CHECK(initializeConfig());
	LSIM_CHECK(Window::initializeWindow(window));
	LSIM_CHECK(GL::initializeOpenGL(window));
	LSIM_CHECK(loadResources());
	LSIM_CHECK(Inputs::initializeInput(inputs, window, editorInputs, scene, sharedState, camera));
	LSIM_CHECK(Scene::initializeScene(sharedState, camera, skybox));
	LSIM_CHECK(initializeSystems());

	#ifdef GAME
	LSIM_CHECK(initializeScripts());
	#endif
	return std::nullopt;
}

std::optional<LSIM::Error> Engine::update() {
	engineLogger("stdInfo", "Starting main gameplay loop");
	while (!glfwWindowShouldClose(window)) {
		updateAspect();
		updateTime();

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		LSIM_CHECK(Inputs::handleInputs(inputs, window, scene, camera, deltaTime));

		#ifndef GAME
		LSIM_CHECK(MeshPicking::handleMeshPicking(window, mouseX, mouseY, windowWidth, windowHeight, camera, sharedState));
		#endif

		LSIM_CHECK(updateSystems());

		#ifdef GAME
		for (auto script : Script::GetAllScripts()) script->Update(deltaTime);
		#endif

		#ifndef GAME
		LSIM_CHECK(EditorGUI::updateGUI(sharedState, scene, skybox, mouseX, mouseY));
		#endif

		glfwSwapBuffers(window);
	}

	return std::nullopt;
}

Engine::~Engine() {
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