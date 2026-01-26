#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <include/scene/scene.h>
#include <include/geometry/terrain.h>
#include <include/geometry/primitives.h>
#include <include/inputs/gui.h>
#include <include/inputs/inputs.h>
#include <include/utils/defaults.h>
#include <include/utils/logging/log.h>
#include <include/utils/json.h>
#include <include/geometry/model.h>

#include <nlohmann/json.hpp>

#include "include/scene/script.h"
#include "include/utils/texture.h"
#include "utils/meshPicking.h"

double mouseX, mouseY;

using json = nlohmann::ordered_json;

json config;

static std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, const int width, const int height){
	glViewport(0, 0, width, height);
}

//Vertices coordinates
std::vector<GLfloat> vertices;

//Indices for vertices order
std::vector<GLuint> indices;

std::vector<std::vector<std::unique_ptr<Mesh>>> meshes;

std::vector<Logger> logs;

static void Log(const std::string& key, const std::string& msg) {
	if (const auto it = loggers.find(key); it != loggers.end())
		(*it->second)(msg, logs);
}

void AddMesh(Scene &scene, const Defaults &defaults, const int selectedMeshType, int &lastClickMesh, const char* workingDir) {
	scene.addMeshSignal = false;

	std::unique_ptr<Mesh> newMesh;

	switch (selectedMeshType) {
		case 0:
			newMesh = std::make_unique<Mesh>(primitives::GenerateCube(1));
			newMesh->name = "Cube";
			break;
		case 1:
			newMesh = std::make_unique<Mesh>(primitives::GeneratePyramid(1));
			newMesh->name = "Pyramid";
			break;
		case 2:
			newMesh = std::make_unique<Mesh>(primitives::GeneratePlane(1));
			newMesh->name = "Plane";
			break;
		case 3:
			newMesh = std::make_unique<Mesh>(primitives::GenerateSphere(defaults.sphereStacks, defaults.sphereSlices, 1));
			newMesh->name = "Sphere";
			break;
		case 4:
			newMesh = std::make_unique<Mesh>(primitives::GenerateTorus(defaults.torusRingSegments, defaults.torusTubeSegments,
			                                                           defaults.torusRingRadius, defaults.torusTubeRadius, 1));

			newMesh->name = "Torus";
			break;
		case 5: {
			std::vector<std::vector<float>> noiseMap = Terrain::GenerateNoiseMap(defaults.size, defaults.size, static_cast<int>(time(nullptr)),
				defaults.scale, defaults.octaves, defaults.persistence, defaults.lacunarity);

			const auto uID = static_cast<long long>(glfwGetTime() * 1'000'000'000LL);
			const char* outputPath = (std::string(workingDir) + "resources/" + std::to_string(uID) + "terrain.png").c_str();
			const GLuint noiseMapTexture = Terrain::noiseMapToTexture(noiseMap, outputPath);

			Terrain::noiseMapToMesh(noiseMap, vertices, indices, defaults.heightScale, defaults.gridScale);

			newMesh = std::make_unique<Mesh>(vertices, indices);
			newMesh->name = "Terrain";
			newMesh->useTexture = true;
			newMesh->texturePath = std::to_string(uID) + "terrain.png";
			newMesh->texId = noiseMapTexture;
			break;
		}
		case 6: {
			const auto filePath = IO::Dialog("Model Files\0*.gltf\0All Files\0*.*\0", GetOpenFileNameA);
			if (filePath.empty()) break; //Exist if FileDIalog was cancelled
			Log("stdInfo", filePath);
			const Model model{(filePath.c_str())};

			std::vector<std::unique_ptr<Mesh>> meshes;

			for (const auto &mesh : model.meshes) {
				auto uniqueMesh = std::make_unique<Mesh>(mesh);
				meshes.push_back(std::move(uniqueMesh));
			}

			auto* node = new Gui::Node{ meshes[0].get(), Gui::root, {} };
			Gui::root->children.push_back(node);

			scene.meshes.push_back(std::move(meshes));
		}
		default:
			break;
	}

	if (newMesh) {
		auto* node = new Gui::Node{ newMesh.get(), Gui::root, {} };
		Gui::root->children.push_back(node);

		scene.meshes.push_back(std::vector<std::unique_ptr<Mesh>>());
		scene.meshes.back().push_back(std::move(newMesh));
	}

	lastClickMesh = scene.meshes.size() - 1;

	Log("stdInfo", "Successfully added mesh");
}

void DeleteMesh(Scene &scene, std::vector<int>& currentMeshes, int &lastClickMesh) {
	scene.deleteMeshSignal = false;

	std::sort(currentMeshes.begin(), currentMeshes.end());
	std::reverse(currentMeshes.begin(), currentMeshes.end());
	for (const int object : currentMeshes) {
		for (const auto &mesh : scene.meshes[object]) {
			const Mesh* meshToDelete = mesh.get();

			if (Gui::Node* nodeToDelete = Gui::FindNodeByMesh(Gui::root, meshToDelete)) {
				Gui::DeleteNode(nodeToDelete); // This handles reparenting children
			}
		}

		scene.meshes.erase(scene.meshes.begin() + object);
	}
	currentMeshes.clear();
	if (lastClickMesh > scene.meshes.size() -1)
		lastClickMesh = scene.meshes.size() - 1;

	Log("stdInfo", "Successfully deleted mesh");
}

void AddLight(Scene &scene, int &currentLight) {
	scene.addLightSignal = false;

	constexpr Light light;
	scene.lights.push_back(light);

	currentLight = scene.lights.size() - 1;

	Log("stdInfo", "Successfully added light");
}

void DeleteLight(Scene &scene, int &currentLight) {
	scene.deleteLightSignal = false;
	scene.lights.erase(scene.lights.begin() + currentLight);

	if (!scene.lights.empty()) {
		currentLight = scene.lights.size() - 1;
		return;
	}
	currentLight = -1;

	Log("stdInfo", "Successfully deleted light");
}

void DrawLights(Shader &shader, Defaults defaults, Scene &scene) {
	for (int i = 0; i < defaults.MAX_LIGHTS; ++i) {
		std::string prefix = "lights[" + std::to_string(i) + "].";
		if (i < scene.lights.size()) {
			glUniform1i(glGetUniformLocation(shader.ID, (prefix + "lightType").c_str()), scene.lights[i].lightType);
			glUniform4fv(glGetUniformLocation(shader.ID, (prefix + "lightColor").c_str()), 1, &scene.lights[i].lightColor[0]);
			glUniform3fv(glGetUniformLocation(shader.ID, (prefix + "lightPos").c_str()), 1, &scene.lights[i].lightPos[0]);
			glUniform3fv(glGetUniformLocation(shader.ID, (prefix + "lightDir").c_str()), 1, &scene.lights[i].lightDir[0]);
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "linear").c_str()), scene.lights[i].linear);
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "quadratic").c_str()), scene.lights[i].quadratic);
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "intensity").c_str()), scene.lights[i].intensity);
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "spotAngle").c_str()), scene.lights[i].spotAngle);
		} else {
			// Clear unused lights
			glUniform1i(glGetUniformLocation(shader.ID, (prefix + "lightType").c_str()), 0);
			glUniform4fv(glGetUniformLocation(shader.ID, (prefix + "lightColor").c_str()), 1, glm::value_ptr(glm::vec4(0.0f)));
			glUniform3fv(glGetUniformLocation(shader.ID, (prefix + "lightPos").c_str()), 1, glm::value_ptr(glm::vec3(0.0f)));
			glUniform3fv(glGetUniformLocation(shader.ID, (prefix + "lightPos").c_str()), 1, glm::value_ptr(glm::vec3(0.0f)));
			glUniform3fv(glGetUniformLocation(shader.ID, (prefix + "lightDir").c_str()), 1, glm::value_ptr(glm::vec3(0.0f)));
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "linear").c_str()), 0.0f);
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "quadratic").c_str()), 0.0f);
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "intensity").c_str()), 0.0f);
			glUniform1f(glGetUniformLocation(shader.ID, (prefix + "spotAngle").c_str()), 0.0f);
		}
	}

	//Ambient
	glUniform4fv(glGetUniformLocation(shader.ID, "ambientLightColour"), 1, glm::value_ptr(scene.ambientLightColour));
	glUniform1f(glGetUniformLocation(shader.ID, "ambientLightIntensity"), scene.ambientLightIntensity);
}

Defaults engineDefaults;

Scene scene {};
Camera camera {};
GLFWwindow* window;
std::string workingDir;


int main(int argc, char** argv)
{

	if (argc >= 2) {
		for (int i = 1; i < argc; ++i) {
			workingDir += argv[i];
			if (i != argc - 1)
				workingDir += " ";
		}
		workingDir += "/";

		Log("stdInfo", "Set working dir to " + workingDir);
	} else {
		Log("stdWarn", "No working directory set");
	}
	//Load config
	engineDefaults = JSONManager::InitJSON(workingDir + "config/config.json", config, loggers);

	//Load shaders

	//default shaders
	std::string vertexShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.vert", config);
	std::string fragmentShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/default.frag", config);

	std::string instanceVertexShader = JSONManager::LoadShaderWithDefines(workingDir + "shaders/instance.vert", config);

	std::string skyboxVert = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.vert", config);
	std::string skyboxFrag = JSONManager::LoadShaderWithDefines(workingDir + "shaders/skybox.frag", config);

	loggers["stdInfo"]->SetModule("MAIN");
	loggers["stdWarn"]->SetModule("MAIN");
	loggers["stdError"]->SetModule("MAIN");

	IO::InitIO();
	Inputs::InitInputs();
	Texture::InitTextures();

	Log("stdInfo", "starting L-SIMENGINE");

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

	Script::InstantiateAll();

	//Error check if the window fails to create
	if (window == nullptr)
	{
		Log("stdError", "Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	Log("stdInfo", "Successfully created the GLFW window");

	//Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	//Specify the viewport of OpenGL in the Window
	glViewport(0, 0, engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight);

	// Register the window resize callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader shaderProgram(vertexShader.c_str(), fragmentShader.c_str(), true);
	Shader instanceShaderProgram(instanceVertexShader.c_str(), fragmentShader.c_str(), true);
	Shader skyboxShaderProgram(skyboxVert.c_str(), skyboxFrag.c_str(), true);

	Gui::Initialize(window);

	meshes.emplace_back();
	meshes.back().push_back(std::make_unique<Mesh>(primitives::GenerateCube(1)));

	meshes.back()[0]->name = "First Cube";
	auto* node = new Gui::Node{ meshes.back()[0].get(), Gui::root, {} };
	Gui::root->children.push_back(node);
	Log("stdInfo", "Successfully created the default \"First Cube\"");

	Log("stdInfo", "Successfully created the default \"First Cube\"");

	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	camera = Camera(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	Log("stdInfo", "Successfully created the camera object");

	std::unordered_map<int, bool> canPress;

	Inputs inputs;
	inputs.defaults = engineDefaults;

	inputs.canPress = canPress;

	float deltaTime = 0.0f;
	float lastTime = 0.0f;
	int lastClickMesh = -1;
	int currentLight = 0;

	scene = Scene{ std::move(meshes), std::move(lights) };
	Log("stdInfo", "Successfully moved meshes and lights into the main scene");

	if (!workingDir.empty()) {
		for (const auto &file : std::filesystem::recursive_directory_iterator(workingDir)) {
			if (file.path().extension().string() == ".lsim") {
				Log("stdInfo", file.path().string());
				std::ifstream LSIMfile(file.path().string(), std::ios::binary);
				scene = IO::loadFromFile(LSIMfile, workingDir);
			}
		}
	}

	//Create skybox
	std::unique_ptr<Mesh> skybox = std::make_unique<Mesh>(primitives::GenerateCube(1));

	//Skybox faces
	std::string faces[6] = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg",
	};

	//Get skybox texture id
	unsigned int skyboxTexId = Texture::GetCubemapId(faces);

	//Run Start() for all scripts
	for (auto script : Script::GetAllScripts()) {
		script->Start();
	}


	//Main render loop
	Log("stdInfo", "Starting main gameplay loop");
	while (!glfwWindowShouldClose(window))
	{
		//Update aspect ratio from current framebuffer size
		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

		//Check if the window is minimized if so skip render loop and just poll events
		if (windowWidth <= 0 || windowHeight <= 0) {
			glfwPollEvents();
			Log("stdInfo", "Window minimized");
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
			lastClickMesh = meshPicking::pickMesh(scene.meshes, camera.Position, rayDir);
		}


		if (scene.addMeshSignal) {
			AddMesh(scene, engineDefaults, selectedMeshType, lastClickMesh, workingDir.c_str());
			Log("stdInfo", "Adding mesh");
		}

		if (scene.deleteMeshSignal) {
			DeleteMesh(scene, currentMeshes, lastClickMesh);
			Log("stdInfo", "Deleting mesh");
		}

		if (scene.addLightSignal && scene.lights.size() < engineDefaults.MAX_LIGHTS) {
			AddLight(scene, currentLight);
			Log("stdInfo", "Adding light");
		} else if (scene.addLightSignal) {
			scene.addLightSignal = false;
			Log("stdWarn", "Tried to create light but it would exceed the maximum number of lights (If you need more lights you can change MAX_LIGHTS in config.json)");
		}

		if (scene.deleteLightSignal) {
			DeleteLight(scene, currentLight);
			Log("stdInfo", "Deleting light");
		}

		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureKeyboard) {
			#ifndef GAME
			if (currentMeshes.empty()) {
				int falseMesh = 0;

				inputs.InputHandler(window, scene, deltaTime, workingDir,
				                    falseMesh, currentLight, selectedMeshType, lastClickMesh, camera.Orientation);
			} else {
				for (int mesh : currentMeshes)
					inputs.InputHandler(window, scene, deltaTime, workingDir,
					                    mesh, currentLight, selectedMeshType, lastClickMesh, camera.Orientation);
			}
			#endif
		}

		shaderProgram.Activate();

		//Draw all meshes
		if (!scene.meshes.empty()) {
			for (auto& objectPtr : scene.meshes) {
				glm::mat4 finalMatrix = objectPtr[0]->modelMatrix;

				for (auto& meshPtr : objectPtr) {
					Mesh& mesh = *meshPtr;

					GLint useTexLoc = glGetUniformLocation(shaderProgram.ID, "useTexture");
					GLint useNormalMapLoc = glGetUniformLocation(shaderProgram.ID, "useNormalMap");
					glUniform1i(useTexLoc, mesh.useTexture);
					glUniform1i(useNormalMapLoc, mesh.useNormalMap);

					glUniform4fv(glGetUniformLocation(shaderProgram.ID, "meshColor"), 1, glm::value_ptr(mesh.color));

					glUniform1f(glGetUniformLocation(shaderProgram.ID, "roughness"), mesh.roughness);
					glUniform3f(glGetUniformLocation(shaderProgram.ID, "F0"), mesh.F0, mesh.F0, mesh.F0);

					auto updatedMatrix = meshPtr->modelMatrix == finalMatrix ? finalMatrix : finalMatrix * meshPtr->modelMatrix;
					mesh.Draw(shaderProgram, camera, updatedMatrix);
				}
			}
		}

		//Switch to instanceShaderProgram to draw instances
		instanceShaderProgram.Activate();

		camera.Matrix(engineDefaults.FOVdeg, engineDefaults.nearPlane, engineDefaults.farPlane, instanceShaderProgram, "camMatrix", aspect);

		DrawLights(instanceShaderProgram, engineDefaults, scene);

		//Draw all instanced meshes
		for (auto& instance : scene.instancedMeshes) {
			Mesh& mesh = *instance->mesh;

			glUniform1i(glGetUniformLocation(instanceShaderProgram.ID, "tex0"), 0);
			glUniform1i(glGetUniformLocation(instanceShaderProgram.ID, "normal0"), 1);

			GLint useTexLoc = glGetUniformLocation(instanceShaderProgram.ID, "useTexture");
			GLint useNormalMapLoc = glGetUniformLocation(instanceShaderProgram.ID, "useNormalMap");
			glUniform1i(useTexLoc, mesh.useTexture);
			glUniform1i(useNormalMapLoc, mesh.useNormalMap);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.texId);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mesh.normalMapId);

			instance->DrawInstances(instanceShaderProgram, camera);
		}

		//Draw skybox
		glDepthFunc(GL_LEQUAL);

		skyboxShaderProgram.Activate();
		camera.Matrix(engineDefaults.FOVdeg, engineDefaults.nearPlane, engineDefaults.farPlane, skyboxShaderProgram, "camMatrix", aspect);

		DrawLights(skyboxShaderProgram, engineDefaults, scene);

		auto view = glm::mat4(1.0f);
		auto projection = glm::mat4(1.0f);
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexId);
		glUniform1i(glGetUniformLocation(skyboxShaderProgram.ID, "skybox"), 0);

		skybox->Draw(skyboxShaderProgram, camera, skybox->modelMatrix);

		glDepthFunc(GL_LESS);

		//Run Update() function for all scripts
		for (auto script : Script::GetAllScripts()) {
			script->Update(deltaTime);
		}

		//Update every mesh
		for (const auto &object : scene.meshes) {
			for (const auto &mesh : object)
				mesh.get()->ApplyTransformations();
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

		Gui::Transform(workingDir, scene.meshes, currentMeshes, selectedMeshType, lastClickMesh);

		Gui::Lighting(scene.lights, currentLight);

		Gui::Scene(skyboxTexId, scene.ambientLightColour, scene.ambientLightIntensity);

		Gui::Debug(mouseX, mouseY);

		ImGui::End();

		ImVec2 hierarchySize(displaySize.x * 0.2f, displaySize.y);
		ImVec2 hierarchyPos(0, 0);

		ImGui::SetNextWindowPos(hierarchyPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(hierarchySize, ImGuiCond_Always);

		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (int clickedMesh = Gui::Hierarchy(scene.meshes); clickedMesh != -1) lastClickMesh = clickedMesh;

		ImGui::End();

		ImVec2 consoleSize(displaySize.x * 0.6f, displaySize.y * 0.3f);
		ImVec2 consolePos(displaySize.x * 0.2f, displaySize.y * 0.7f);

		ImGui::SetNextWindowPos(consolePos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(consoleSize, ImGuiCond_Always);

		ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Gui::Console(selectedLogLevel, logs);

		ImGui::End();

		Gui::End();
		#endif

		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}
	Log("stdInfo", "Exiting L-SIMENGINE");

	Gui::DeleteNodeRecursively(Gui::root);

	Gui::CleanUp();

	shaderProgram.Delete();
	instanceShaderProgram.Delete();
	//Delete window before ending the program
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();

	Log("stdInfo", "Successfully exited L-SIMENGINE");

	return 0;
}