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

#include <nlohmann/json.hpp>

double mouseX, mouseY;

using json = nlohmann::json;

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

std::vector<std::unique_ptr<Mesh>> meshes;

static void Log(const std::string& key, const std::string& msg) {
	if (const auto it = loggers.find(key); it != loggers.end())
		(*it->second)(msg);
}

void AddMesh(Scene &scene, Defaults defaults, const int selectedMeshType, int &lastClickMesh) {
	scene.addMeshSignal = false;

	std::unique_ptr<Mesh> newMesh;

	switch (selectedMeshType) {
		case 0:
			newMesh = std::make_unique<Mesh>(primitives::GenerateCube());
			newMesh->name = "Cube";
			break;
		case 1:
			newMesh = std::make_unique<Mesh>(primitives::GeneratePyramid());
			newMesh->name = "Pyramid";
			break;
		case 2:
			newMesh = std::make_unique<Mesh>(primitives::GeneratePlane());
			newMesh->name = "Plane";
			break;
		case 3:
			newMesh = std::make_unique<Mesh>(primitives::GenerateSphere(20, 30));
			newMesh->name = "Sphere";
			break;
		case 4:
			newMesh = std::make_unique<Mesh>(primitives::GenerateTorus(40, 20, 1, 0.3));
			newMesh->name = "Torus";
			break;
		case 5: {
			std::vector<std::vector<float>> noiseMap = GenerateNoiseMap(256, 256, static_cast<int>(time(nullptr)), 15.0f, 8, 0.5f, 2.0f);

			const GLuint noiseMapTexture = noiseMapToTexture(noiseMap);

			noiseMapToMesh(noiseMap, vertices, indices, 80, defaults.gridScale);

			newMesh = std::make_unique<Mesh>(vertices, indices);
			newMesh->name = "Terrain";
			newMesh->useTexture = true;
			newMesh->texId = noiseMapTexture;
			break;
		}
		default:
			break;
	}

	if (newMesh) {
		auto* node = new Gui::Node{ newMesh.get(), Gui::root, {} };
		Gui::root->children.push_back(node);

		scene.meshes.push_back(std::move(newMesh));

		lastClickMesh = scene.meshes.size() - 1;
	}

	Log("stdInfo", "Successfully added mesh");
}

void DeleteMesh(Scene &scene, std::vector<int>& currentMeshes, int &lastClickMesh) {
	scene.deleteMeshSignal = false;

	std::sort(currentMeshes.begin(), currentMeshes.end());
	std::reverse(currentMeshes.begin(), currentMeshes.end());
	for (const int mesh : currentMeshes) {
		const Mesh* meshToDelete = scene.meshes[mesh].get();

		if (Gui::Node* nodeToDelete = Gui::FindNodeByMesh(Gui::root, meshToDelete)) {
			Gui::DeleteNode(nodeToDelete); // This handles reparenting children
		}

		scene.meshes.erase(scene.meshes.begin() + mesh);
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

int main()
{
	//Load config
	const Defaults engineDefaults = JSONManager::InitJSON("config/config.json", config, loggers);

	std::string vertexShader = JSONManager::LoadShaderWithDefines("shaders/default.vert", config);
	std::string fragmentShader = JSONManager::LoadShaderWithDefines("shaders/default.frag", config);

	loggers["stdInfo"]->SetModule("MAIN");
	loggers["stdWarn"]->SetModule("MAIN");
	loggers["stdError"]->SetModule("MAIN");

	IO::InitIO();
	Inputs::InitInputs();

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
	GLFWwindow* window = glfwCreateWindow(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, "L-SIM ENGINE", nullptr, nullptr);

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

	//Generate Shader object using shaders default.vert and default.frag
	Shader shaderProgram(vertexShader.c_str(), fragmentShader.c_str(), true);

	Gui::Initialize(window);

	meshes.push_back(std::make_unique<Mesh>(primitives::GenerateCube()));
	meshes.back()->name = "First Cube";
	auto* node = new Gui::Node{ meshes.back().get(), Gui::root, {} };
	Gui::root->children.push_back(node);
	Log("stdInfo", "Successfully created the default \"First Cube\"");

	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	Camera camera(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));
	Log("stdInfo", "Successfully created the camera object");

	std::unordered_map<int, bool> canPress;

	Inputs inputs;

	inputs.canPress = canPress;

	float deltaTime = 0.0f;
	float lastTime = 0.0f;
	int lastClickMesh = -1;
	int currentLight = 0;

	Scene scene = {std::move(meshes), std::move(lights)};
	Log("stdInfo", "Successfully moved meshes and lights into the main scene");

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

		for (int i = 0; i < engineDefaults.MAX_LIGHTS; ++i) {
			std::string prefix = "lights[" + std::to_string(i) + "].";
			if (i < scene.lights.size()) {
				glUniform4fv(glGetUniformLocation(shaderProgram.ID, (prefix + "lightColor").c_str()), 1, &scene.lights[i].lightColor[0]);
				glUniform3fv(glGetUniformLocation(shaderProgram.ID, (prefix + "lightPos").c_str()), 1, &scene.lights[i].lightPos[0]);
				glUniform1f(glGetUniformLocation(shaderProgram.ID, (prefix + "linear").c_str()), scene.lights[i].linear);
				glUniform1f(glGetUniformLocation(shaderProgram.ID, (prefix + "quadratic").c_str()), scene.lights[i].quadratic);
			} else {
				// Clear unused lights
				glUniform4fv(glGetUniformLocation(shaderProgram.ID, (prefix + "lightColor").c_str()), 1, glm::value_ptr(glm::vec4(0.0f)));
				glUniform3fv(glGetUniformLocation(shaderProgram.ID, (prefix + "lightPos").c_str()), 1, glm::value_ptr(glm::vec3(0.0f)));
				glUniform1f(glGetUniformLocation(shaderProgram.ID, (prefix + "linear").c_str()), 0.0f);
				glUniform1f(glGetUniformLocation(shaderProgram.ID, (prefix + "quadratic").c_str()), 0.0f);
			}
		}



		//Handle camera inputs

		// Only process camera movement if ImGui is not using the mouse
		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
			camera.Inputs(window, deltaTime);
		}


		glfwGetCursorPos(window, &mouseX, &mouseY);

		camera.Matrix(45.0f, 0.1f, 1000000.0f, shaderProgram, "camMatrix", aspect);

		static std::vector currentMeshes = {0};
		static int selectedMeshType = 0;


		if (scene.addMeshSignal) {
			AddMesh(scene, engineDefaults, selectedMeshType, lastClickMesh);
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

			if (currentMeshes.empty()) {
				int falseMesh = 0;

				inputs.InputHandler(window, scene, falseMesh,
					currentLight, selectedMeshType, lastClickMesh, camera.Orientation);
			} else {
				for (int mesh : currentMeshes)
					inputs.InputHandler(window, scene, mesh,
						currentLight, selectedMeshType, lastClickMesh, camera.Orientation);
			}
		}

		if (!scene.meshes.empty()) {
			for (auto& meshPtr : scene.meshes) {
				Mesh& mesh = *meshPtr;

				GLint useTexLoc = glGetUniformLocation(shaderProgram.ID, "useTexture");
				glUniform1i(useTexLoc, mesh.useTexture);

				glUniform4fv(glGetUniformLocation(shaderProgram.ID, "meshColor"), 1, glm::value_ptr(mesh.color));

				mesh.Draw(shaderProgram, camera);
			}
		}



		Gui::Begin();

		ImGui::SetNextWindowPos(ImVec2(1620, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 1080), ImGuiCond_Always);

		ImGui::Begin("Main UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Gui::Transform(scene.meshes, currentMeshes, selectedMeshType, lastClickMesh);

		Gui::Lighting(scene.lights, currentLight);

		Gui::Debug(mouseX, mouseY);

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 1080), ImGuiCond_Always);

		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (int clickedMesh = Gui::Hierarchy(scene.meshes); clickedMesh != -1) lastClickMesh = clickedMesh;

		ImGui::End();

		Gui::End();

		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}
	Log("stdInfo", "Exiting L-SIMENGINE");

	Gui::DeleteNodeRercursively(Gui::root);

	Gui::CleanUp();

	shaderProgram.Delete();
	//Delete window before ending the program
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();

	Log("stdInfo", "Successfully exited L-SIMENGINE");

	return 0;
}