#include<iostream>
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

std::vector<Mesh> meshes;

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

	Mesh cube = primitives::GenerateCube();



	meshes.push_back(cube);


	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	std::unordered_map<int, bool> canPress;

	Inputs inputs;

	inputs.canPress = canPress;

	//Main render loop
	while (!glfwWindowShouldClose(window))
	{
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
		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureMouse) {
			camera.Inputs(window);
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		//Update aspect ratio from current framebuffer size
		int windowWidth, windowHeight;

		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

		camera.Matrix(45.0f, 0.1f, 1000000.0f, shaderProgram, "camMatrix", aspect);

		static int currentMesh = 0;
		static int selectedMeshType = 0;
		static bool deletePressed = false;
		static bool fPressed = false;



		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
			Mesh mesh = primitives::GenerateCube();

			switch (selectedMeshType) {
				case 0:
					mesh = primitives::GenerateCube();
					break;
				case 1:
					mesh = primitives::GeneratePyramid();
					break;
				case 2:
					mesh = primitives::GeneratePlane();
					break;
				case 3:
					mesh = primitives::GenerateSphere(20, 30);
					break;
				case 4:
					mesh = primitives::GenerateTorus(40, 20, 30, 10);
					break;
				case 5: {
					std::vector<std::vector<float>> noiseMap = GenerateNoiseMap(1024, 1024, static_cast<int>(time(nullptr)), 15.0f, 8, 0.5f, 2.0f);

					GLuint noiseMapTexture = noiseMapToTexture(noiseMap);

					noiseMapToMesh(noiseMap, vertices, indices, 80, 2.0f);

					Mesh terrain(vertices, indices);

					terrain.useTexture = true;
					terrain.texId = noiseMapTexture;

					meshes.push_back(terrain);
				}
				default:
					break;
			}
			meshes.push_back(mesh);
			fPressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && !meshes.empty() && !deletePressed) {
			if (currentMesh >= 0 && currentMesh < meshes.size()) {
				meshes.erase(meshes.begin() + currentMesh);

				if (currentMesh >= meshes.size()) {
					currentMesh = static_cast<int>(meshes.size()) - 1;
				}
			}
			currentMesh = 0;
			deletePressed = true;
		}

		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_RELEASE)
			deletePressed = false;

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
			fPressed = false;

		if (ImGuiIO& io = ImGui::GetIO(); !io.WantCaptureKeyboard)
			inputs.InputHandler(window, lightPos, meshes, currentMesh, selectedMeshType);

		for (auto & mesh : meshes) {

			GLint useTexLoc = glGetUniformLocation(shaderProgram.ID, "useTexture");
			glUniform1i(useTexLoc, mesh.useTexture);

			glUniform4fv(glGetUniformLocation(shaderProgram.ID, "meshColor"), 1, glm::value_ptr(mesh.color));

			mesh.Draw(shaderProgram, camera);
		}

		Gui::Begin();

		ImGui::SetNextWindowPos(ImVec2(1620, 0), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(300, 1080), ImGuiCond_Always);

		ImGui::Begin("Main UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMove);

		Gui::Transform(meshes, currentMesh, selectedMeshType);

		Gui::Lighting(lightColor, lightPos, attenuationScale);

		Gui::Debug(mouseX, mouseY);

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