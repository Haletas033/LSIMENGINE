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

#include <nlohmann/json.hpp>

double mouseX, mouseY;

using json = nlohmann::json;

json config;

std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, const int width, const int height){
	glViewport(0, 0, width, height);
}

void LoadJSON(const std::string &path) {
	std::ifstream file(path);
	if (!file.is_open()) throw std::runtime_error("Failed to open config.json");
	file >> config;
}

Defaults LoadConfigDefaults(json config) {
	Defaults configDefaults;

	configDefaults.MAX_LIGHTS = config["shader-constants"]["MAX_LIGHTS"].get<unsigned int>();
	configDefaults.defaultWindowWidth = config["defaults"]["defaultWindowWidth"].get<unsigned int>();
	configDefaults.defaultWindowHeight = config["defaults"]["defaultWindowHeight"].get<unsigned int>();
	configDefaults.gridScale = config["defaults"]["gridScale"].get<float>();

	return configDefaults;
}

std::string LoadShaderWithDefines(const std::string &path) {
	std::ifstream file(path);
	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string defines = "#version 330 core\n";

	// Inject defines into the shader
	for (const auto&[name, value] : config["shader-constants"].items())
		defines += "#define " + name + " " + std::to_string(value.get<int>()) + "\n";

	return defines + buffer.str();
}

void LoadLoggers(json config) {
	auto loggersJson = config["loggers"];
	for (const auto &[loggerName, loggerValue] : loggersJson.items()) {
		auto logger = std::make_unique<Logger>();

		for (const auto &[fieldName, fieldValue] : loggerValue.items()) {
			if (fieldName == "hasTimeStamp") {
				logger->HasTimeStamp();
			} else if (fieldName == "colour") {
				logger->SetColour(fieldValue);
			} else if (fieldName == "type") {
				logger->SetType(fieldValue);
			}
		}

		loggers[loggerName] = std::move(logger);
	}
}

//Vertices coordinates
std::vector<GLfloat> vertices;

//Indices for vertices order
std::vector<GLuint> indices;

std::vector<std::unique_ptr<Mesh>> meshes;

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
}

void AddLight(Scene &scene, int &currentLight) {
	scene.addLightSignal = false;

	constexpr Light light;
	scene.lights.push_back(light);

	currentLight = scene.lights.size() - 1;
}

void DeleteLight(Scene &scene, int &currentLight) {
	scene.deleteLightSignal = false;
	scene.lights.erase(scene.lights.begin() + currentLight);

	if (!scene.lights.empty()) {
		currentLight = scene.lights.size() - 1;
		return;
	}
	currentLight = -1;
}

inline void log(const std::string& key, const std::string& msg) {
	if (auto it = loggers.find(key); it != loggers.end())
		(*it->second)(msg);
}

int main()
{
	//Load config
	LoadJSON("config/config.json");

	LoadLoggers(config);

	log("stdInfo", "Hello, Info");
	log("stdWarn", "Hello, Warning");
	log("stdError", "Hello, Error");


	std::string vertexShader = LoadShaderWithDefines("shaders/default.vert");
	std::string fragmentShader = LoadShaderWithDefines("shaders/default.frag");

	const Defaults engineDefaults = LoadConfigDefaults(config);


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
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
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

	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	Camera camera(engineDefaults.defaultWindowWidth, engineDefaults.defaultWindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	std::unordered_map<int, bool> canPress;

	Inputs inputs;

	inputs.canPress = canPress;

	float deltaTime = 0.0f;
	float lastTime = 0.0f;
	int lastClickMesh = -1;
	int currentLight = 0;

	Scene scene = {std::move(meshes), std::move(lights)};

	//Main render loop
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
		}

		if (scene.deleteMeshSignal) {
			DeleteMesh(scene, currentMeshes, lastClickMesh);
		}

		if (scene.addLightSignal && scene.lights.size() < engineDefaults.MAX_LIGHTS) {
			AddLight(scene, currentLight);
		}

		if (scene.deleteLightSignal) {
			DeleteLight(scene, currentLight);
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

	Gui::DeleteNodeRercursively(Gui::root);

	Gui::CleanUp();

	shaderProgram.Delete();
	//Delete window before ending the program
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();

	return 0;
}