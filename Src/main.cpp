#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <include/scene/scene.h>
#include <include/geometry/terrain.h>
#include <include/geometry/primitive.h>
#include <include/inputs/gui.h>
#include <include/inputs/inputs.h>
#include <include/utils/defaults.h>
#include <include/utils/logging/log.h>
#include <include/utils/json.h>
#include <include/geometry/model.h>

#include <nlohmann/json.hpp>

#include "ECS/name.h"
#include "ECS/system.h"
#include "editor/editorInputs.h"
#include "geometry/transformSystem.h"
#include "include/scene/script.h"
#include "include/utils/texture.h"
#include "rendering/renderSystem.h"
#include "utils/meshPicking.h"

double mouseX, mouseY;

using json = nlohmann::ordered_json;

json config;

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, const int width, const int height){
	glViewport(0, 0, width, height);
}

void AddLight(Scene &scene, int &currentLight) {
	scene.addLightSignal = false;

	constexpr Light light;
	scene.lights.push_back(light);

	currentLight = scene.lights.size() - 1;

	engineLogger("stdInfo", "Successfully added light");
}

void DeleteLight(Scene &scene, int &currentLight) {
	scene.deleteLightSignal = false;
	scene.lights.erase(scene.lights.begin() + currentLight);

	if (!scene.lights.empty()) {
		currentLight = scene.lights.size() - 1;
		return;
	}
	currentLight = -1;

	engineLogger("stdInfo", "Successfully deleted light");
}

void DrawLights(Shader &shader, Defaults defaults, Scene &scene) {
	for (int i = 0; i < defaults.MAX_LIGHTS; ++i) {
		std::string prefix = "lights[" + std::to_string(i) + "].";
		if (i < scene.lights.size()) {
			shader.SetInt(prefix + "lightType", scene.lights[i].lightType);
			shader.SetVec4(prefix + "lightColor", 1, &scene.lights[i].lightColor[0]);
			shader.SetVec3(prefix + "lightPos", 1, &scene.lights[i].lightPos[0]);
			shader.SetVec3(prefix + "lightDir", 1, &scene.lights[i].lightDir[0]);
			shader.SetFloat(prefix + "linear", scene.lights[i].linear);
			shader.SetFloat(prefix + "quadratic", scene.lights[i].quadratic);
			shader.SetFloat(prefix + "intensity", scene.lights[i].intensity);
			shader.SetFloat(prefix + "spotAngle", scene.lights[i].spotAngle);
		} else {
			// Clear unused lights
			shader.SetInt(prefix + "lightType", 0);
			shader.SetVec4(prefix + "lightColor", 1, glm::value_ptr(glm::vec4(0.0f)));
			shader.SetVec3(prefix + "lightPos", 1, glm::value_ptr(glm::vec3(0.0f)));
			shader.SetVec3(prefix + "lightPos", 1, glm::value_ptr(glm::vec3(0.0f)));
			shader.SetVec3(prefix + "lightDir", 1, glm::value_ptr(glm::vec3(0.0f)));
			shader.SetFloat(prefix + "linear", 0.0f);
			shader.SetFloat(prefix + "quadratic", 0.0f);
			shader.SetFloat(prefix + "intensity", 0.0f);
			shader.SetFloat(prefix + "spotAngle", 0.0f);
		}
	}

	shader.SetVec4("ambientLightColour", 1, glm::value_ptr(scene.ambientLightColour));
	shader.SetFloat("ambientLightIntensity", scene.ambientLightIntensity);
}

Defaults engineDefaults;

Scene scene {};
Camera camera {};
GLFWwindow* window;
std::string workingDir;
Registry registry;
MeshPool meshPool;
std::vector<std::unique_ptr<System>> systems;

int main(int argc, char** argv) {
	if (argc >= 2) {
		for (int i = 1; i < argc; ++i) {
			workingDir += argv[i];
			if (i != argc - 1)
				workingDir += " ";
		}
		workingDir += "/";
	}
	//Load config
	engineDefaults = JSONManager::InitJSON(workingDir + "config/config.json", config);
	SharedState sharedState{};
	Logger::InitEngineLogger();

	//Load shaders

	//default shaders
	std::string vertexShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.vert", config);
	std::string fragmentShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.frag", config);

	std::string instanceVertexShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/instance.vert", config);

	std::string skyboxVert = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.vert", config);
	std::string skyboxFrag = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.frag", config);

	Inputs inputs{};
	EditorInputs editorInputs{};

	IO::InitIO();
	Texture::InitTextures();

	engineLogger("stdInfo", "starting L-SIMENGINE");

	//Initialize GLFW
	glfwInit();

	//Tell GLFW what version of OpenGL we are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Tell GLFW we are using the CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::vector<Light> lights;
	Light light1;
	lights.push_back(light1);

	//Create a GLFW window object of 800 by 800 pixels
	window = glfwCreateWindow(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, ("L-SIM ENGINE " + engineDefaults.version + " "+ workingDir).c_str(), nullptr, nullptr);

	//Error check if the window fails to create
	if (window == nullptr) {
		engineLogger("stdError", "Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	inputs.InitInputs(window);
	editorInputs.Init(registry, meshPool, scene, workingDir, sharedState, engineDefaults, camera, inputs);

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shaderProgram({vertexShader, fragmentShader});
	ResourceManager::addShader("PBRShader", std::move(shaderProgram));

	Shader instanceShaderProgram({instanceVertexShader, fragmentShader});
	Shader skyboxShaderProgram({skyboxVert, skyboxFrag});

	systems.push_back(std::make_unique<TransformSystem>());
	systems.push_back(std::make_unique<RenderSystem>(meshPool, camera));

	Gui::Initialize(window);

	EntityHandle firstCube = Mesh::create(Primitive::CUBE, MeshMode::STATIC, registry, meshPool, Material::createStandardPBR(), Transform());
	sharedState.current_meshes() = {firstCube};
	registry.getComponent<Name>(firstCube)->value = "First Cube";
	auto* node = new Gui::Node{ firstCube, Gui::root, {} };
	Gui::root->children.push_back(node);

	engineLogger("stdInfo", "Successfully created the default \"First Cube\"");

	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	camera = Camera(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	engineLogger("stdInfo", "Successfully created the camera object");

	float deltaTime = 0.0f;
	float lastTime = 0.0f;
	int currentLight = 0;

	scene = Scene{ std::move(lights) };
	engineLogger("stdInfo", "Successfully moved meshes and lights into the main scene");

	if (!workingDir.empty()) {
		for (const auto &file : std::filesystem::recursive_directory_iterator(workingDir)) {
			if (file.path().extension().string() == ".lsim") {
				engineLogger("stdInfo", file.path().string());
				std::ifstream LSIMfile(file.path().string(), std::ios::binary);
				scene = IO::loadFromFile(LSIMfile, workingDir);
			}
		}
	}

	//Create skybox
	// std::unique_ptr<Mesh> skybox = std::make_unique<Mesh>(Primitive::GenerateCube(1));

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
	unsigned int skyboxTexId = Texture::GetCubemapId(faces, GL_LINEAR);

	//Run Start() for all scripts
	for (auto script : Script::GetAllScripts()) {
		script->Start();
	}


	//Main render loop
	engineLogger("stdInfo", "Starting main gameplay loop");
	while (!glfwWindowShouldClose(window))
	{
		//Update aspect ratio from current framebuffer size
		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

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
		//Tells OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		DrawLights(shaderProgram, engineDefaults, scene);

		//Handle camera inputs

		// Only process camera movement if ImGui is not using the mouse
		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
			#ifndef GAME
			camera.Inputs(window, deltaTime);
			#endif
		}


		glfwGetCursorPos(window, &mouseX, &mouseY);

		camera.Matrix(engineDefaults.FOVdeg, engineDefaults.nearPlane, engineDefaults.farPlane, shaderProgram, "camMatrix", aspect);

		static std::vector currentMeshes = {0};
		static int selectedMeshType = 0;
		static int selectedLogLevel = 0;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			auto viewport = glm::vec4(0.0f, 0.0f, windowWidth, windowHeight);
			auto rayDir = meshPicking::GetMouseRay(mouseX, mouseY, camera.projection, camera.view, viewport);
			if (const auto e = meshPicking::pickMesh(registry, camera.Position, rayDir); e.has_value()) {
				sharedState.current_meshes() = {e.value()};
			}
		}

		if (scene.addLightSignal && scene.lights.size() < engineDefaults.MAX_LIGHTS) {
			AddLight(scene, currentLight);
			engineLogger("stdInfo", "Adding light");
		} else if (scene.addLightSignal) {
			scene.addLightSignal = false;
			engineLogger("stdWarn", "Tried to create light but it would exceed the maximum number of lights (If you need more lights you can change MAX_LIGHTS in config.json)");
		}

		if (scene.deleteLightSignal) {
			DeleteLight(scene, currentLight);
			engineLogger("stdInfo", "Deleting light");
		}

		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureKeyboard) {
			#ifndef GAME
			inputs.handleInputs((Inputs::InputContext){scene, deltaTime});
			#endif
		}

		shaderProgram.Activate();

		//Draw all meshes
		for (auto& sys : systems) sys->update(registry, deltaTime);

		// //Switch to instanceShaderProgram to draw instances
		// instanceShaderProgram.Activate();
		//
		// camera.Matrix(engineDefaults.FOVdeg, engineDefaults.nearPlane, engineDefaults.farPlane, instanceShaderProgram, "camMatrix", aspect);
		//
		// DrawLights(instanceShaderProgram, engineDefaults, scene);
		//
		// //Draw all instanced meshes
		// for (auto& instance : scene.instancedMeshes) {
		// 	Mesh& mesh = *instance->mesh;
		//
		// 	instanceShaderProgram.SetInt("tex0", 0);
		// 	instanceShaderProgram.SetInt("normal0", 1);
		//
		// 	GLint useTexLoc = shaderProgram.GetLocation("useTexture");
		// 	GLint useNormalMapLoc = shaderProgram.GetLocation("useNormalMap");
		// 	glUniform1i(useTexLoc, mesh.useTexture);
		// 	glUniform1i(useNormalMapLoc, mesh.useNormalMap);
		//
		// 	glActiveTexture(GL_TEXTURE0);
		// 	glBindTexture(GL_TEXTURE_2D, mesh.texId);
		//
		// 	glActiveTexture(GL_TEXTURE1);
		// 	glBindTexture(GL_TEXTURE_2D, mesh.normalMapId);
		//
		// 	instance->DrawInstances(instanceShaderProgram, camera);
		// }

		//Draw skybox
		glDepthFunc(GL_LEQUAL);

		skyboxShaderProgram.Activate();
		camera.Matrix(engineDefaults.FOVdeg, engineDefaults.nearPlane, engineDefaults.farPlane, skyboxShaderProgram, "camMatrix", aspect);

		DrawLights(skyboxShaderProgram, engineDefaults, scene);

		auto view = glm::mat4(1.0f);
		auto projection = glm::mat4(1.0f);
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		skyboxShaderProgram.SetMat4("view", 1, glm::value_ptr(view));
		skyboxShaderProgram.SetMat4("projection", 1, glm::value_ptr(projection));

		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexId);
		skyboxShaderProgram.SetInt("skybox", 0);

		// skybox->Draw(skyboxShaderProgram, camera, skybox->modelMatrix);

		glDepthFunc(GL_LESS);

		//Run Update() function for all scripts
		for (auto script : Script::GetAllScripts()) {
			script->Update(deltaTime);
		}

		//Update every light
		for (auto &light : scene.lights) {
			// Recalculate dependent values even if the GUI is closed
			light.invScale  = 1.0f / (light.attenuationScale + 0.001f);
			light.linear    = 0.09f  * light.invScale;
			light.quadratic = 0.032f * light.invScale;
		}


		#ifndef GAME
		Gui::Begin();

		ImVec2 displaySize = ImGui::GetIO().DisplaySize; // (width, height)

		ImVec2 mainUISize(displaySize.x * 0.2f, displaySize.y);
		ImVec2 mainUIPos(displaySize.x - mainUISize.x, 0);

		ImGui::SetNextWindowPos(mainUIPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(mainUISize, ImGuiCond_Always);

		ImGui::Begin("Main UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Gui::Transform(registry, sharedState, workingDir,selectedMeshType);

		Gui::Lighting(scene.lights, currentLight);

		Gui::SceneGUI(workingDir, skyboxTexId, scene.ambientLightColour, scene.ambientLightIntensity);

		Gui::Debug(mouseX, mouseY);

		ImGui::End();

		ImVec2 hierarchySize(displaySize.x * 0.2f, displaySize.y);
		ImVec2 hierarchyPos(0, 0);

		ImGui::SetNextWindowPos(hierarchyPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(hierarchySize, ImGuiCond_Always);

		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (std::optional<EntityHandle> clickedMesh = Gui::Hierarchy(registry); clickedMesh) sharedState.current_meshes() = {clickedMesh.value()};

		ImGui::End();

		ImVec2 consoleSize(displaySize.x * 0.6f, displaySize.y * 0.3f);
		ImVec2 consolePos(displaySize.x * 0.2f, displaySize.y * 0.7f);

		ImGui::SetNextWindowPos(consolePos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(consoleSize, ImGuiCond_Always);

		ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Gui::Console(selectedLogLevel);

		ImGui::End();

		Gui::End();
		#endif

		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}
	engineLogger("stdInfo", "Exiting L-SIMENGINE");

	Gui::DeleteNodeRecursively(registry, Gui::root);
	Gui::CleanUp();

	systems.clear();
	registry = Registry{};
	meshPool = MeshPool{};
	ResourceManager::clear();

	glfwDestroyWindow(window);
	glfwTerminate();

	engineLogger("stdInfo", "Successfully exited L-SIMENGINE");
	return 0;
}