#include <algorithm>
#include<iostream>
#include <memory>
#include <unordered_map>

#include"../include/terrain.h"
#include"../include/mesh.h"
#include"../include/primitives.h"
#include"../include/gui.h"
#include"../include/inputs.h"

constexpr unsigned int width = 1920;
constexpr unsigned int height = 1080;

float gridScale = 2.0f;

double mouseX, mouseY;

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, const int width, const int height){
	glViewport(0, 0, width, height);
}

//Vertices coordinates
std::vector<GLfloat> vertices;

//Indices for vertices order
std::vector<GLuint> indices;

std::vector<std::unique_ptr<Mesh>> meshes;

int main()
{
	//Initialize GLFW
	glfwInit();

	//Tell GLFW what version of OpenGL we are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Tell GLFW we are using the CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




	//Create a GLFW window object of 800 by 800 pixels
	GLFWwindow* window = glfwCreateWindow(width, height, "L-SIM ENGINE", nullptr, nullptr);

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
	glViewport(0, 0, width, height);

	// Register the window resize callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Generate Shader object using shaders default.vert and default.frag
	Shader shaderProgram("../shaders/default.vert", "../shaders/default.frag");

	Gui::Initialize(window);

	auto lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	auto lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

	meshes.push_back(std::make_unique<Mesh>(primitives::GenerateCube()));
	meshes.back()->name = "First Cube";

	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	std::unordered_map<int, bool> canPress;

	Inputs inputs;

	inputs.canPress = canPress;

	float deltaTime = 0.0f;
	float lastTime = 0.0f;
	int lastClickMesh = -1;

	//Main render loop
	while (!glfwWindowShouldClose(window))
	{
		//Update aspect ratio from current framebuffer size
		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

		//Check if the window is minimized if so skip renderloop and just poll events
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

		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		static float attenuationScale = 1.0f;

		float invScale = 1.0f / (attenuationScale + 0.001f); // Avoid division by zero
		float linear     = 0.09f * invScale;
		float quadratic  = 0.032f * invScale;

		glUniform1f(glGetUniformLocation(shaderProgram.ID, "linear"), linear);
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "quadratic"), quadratic);


		//Handle camera inputs

		// Only process camera movement if ImGui is not using the mouse
		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureMouse && !io.WantCaptureKeyboard) {
			camera.Inputs(window, deltaTime);
		}


		glfwGetCursorPos(window, &mouseX, &mouseY);

		camera.Matrix(45.0f, 0.1f, 1000000.0f, shaderProgram, "camMatrix", aspect);

		static std::vector currentMeshes = {0};
		static int selectedMeshType = 0;
		static bool deletePressed = false;
		static bool fPressed = false;



		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
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

					GLuint noiseMapTexture = noiseMapToTexture(noiseMap);

					noiseMapToMesh(noiseMap, vertices, indices, 80, 2.0f);

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


				meshes.push_back(std::move(newMesh));
			}

			fPressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && !meshes.empty() && !deletePressed){
			std::sort(currentMeshes.begin(), currentMeshes.end());
			std::reverse(currentMeshes.begin(), currentMeshes.end());
			for (int mesh : currentMeshes) {
				Mesh* meshToDelete = meshes[mesh].get();

				if (Gui::Node* nodeToDelete = Gui::FindNodeByMesh(Gui::root, meshToDelete)) {
					Gui::DeleteNode(nodeToDelete); // This handles reparenting children
				}

				meshes.erase(meshes.begin() + mesh);
			}
			currentMeshes.clear();
			lastClickMesh = -1;
			deletePressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_RELEASE)
			deletePressed = false;

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
			fPressed = false;

		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureKeyboard) {
			if (currentMeshes.empty()) {
				int falseMesh = 0;
				inputs.InputHandler(window, lightPos, meshes, falseMesh, selectedMeshType, camera.Orientation);
			} else {
				for (int mesh : currentMeshes)
					inputs.InputHandler(window, lightPos, meshes, mesh, selectedMeshType, camera.Orientation);
			}
		}

		for (auto& meshPtr : meshes) {
			Mesh& mesh = *meshPtr;

			GLint useTexLoc = glGetUniformLocation(shaderProgram.ID, "useTexture");
			glUniform1i(useTexLoc, mesh.useTexture);

			glUniform4fv(glGetUniformLocation(shaderProgram.ID, "meshColor"), 1, glm::value_ptr(mesh.color));

			mesh.Draw(shaderProgram, camera);
		}



		Gui::Begin();

		ImGui::SetNextWindowPos(ImVec2(1620, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 1080), ImGuiCond_Always);

		ImGui::Begin("Main UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Gui::Transform(meshes, currentMeshes, selectedMeshType, lastClickMesh);

		Gui::Lighting(lightColor, lightPos, attenuationScale);

		Gui::Debug(mouseX, mouseY);

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 1080), ImGuiCond_Always);

		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		if (int clickedMesh = Gui::Hierarchy(meshes); clickedMesh != -1) lastClickMesh = clickedMesh;

		ImGui::End();

		Gui::End();

		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}

	Gui::CleanUp();

	shaderProgram.Delete();
	//Delete window before ending the program
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();

	return 0;
}