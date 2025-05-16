#include<iostream>
#include"../include/terrain.h"
#include"../include/mesh.h"
#include"../include/primitives.h"
#include"../include/gui.h"

const unsigned int width = 1920;
const unsigned int height = 1080;

float gridScale = 2.0f;

double mouseX, mouseY;

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
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




	//Create a GLFWwindow object of 800 by 800 pixels
	GLFWwindow* window = glfwCreateWindow(width, height, "LSIMENGINE", nullptr, nullptr);

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

	Gui gui;
	gui.Initialize(window);


	std::vector<std::vector<float>> noiseMap = GenerateNoiseMap(1024, 1024, static_cast<unsigned int>(time(nullptr)), 15.0f, 8, 0.5f, 2.0f);

	GLuint noiseMapTexture = noiseMapToTexture(noiseMap);

	noiseMapToMesh(noiseMap, vertices, indices, 80, 2.0f);

	primitives p;


	Mesh terrain(vertices, indices);

	Mesh cube = p.GenerateCube();

	meshes.push_back(cube);
	meshes.push_back(terrain);


	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	//Main while loop
	while (!glfwWindowShouldClose(window))
	{
		//Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Tells OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		//Handle camera inputs
		ImGuiIO& io = ImGui::GetIO();

		// Only process camera movement if ImGui is not using the mouse
		if (!io.WantCaptureMouse) {
			camera.Inputs(window);
		}

		glfwGetCursorPos(window, &mouseX, &mouseY);

		//Update aspect ratio from current framebuffer size
		int windowWidth, windowHeight;

		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		float aspect = (float)windowWidth / (float)windowHeight;

		camera.Matrix(45.0f, 0.1f, 1000000.0f, shaderProgram, "camMatrix", aspect);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, noiseMapTexture);
		//Set the sampler uniform to use texture unit 0
		GLint texLoc = glGetUniformLocation(shaderProgram.ID, "tex0");
		glUniform1i(texLoc, 0);

		for (size_t i = 0; i < meshes.size(); ++i) {
			auto& mesh = meshes[i];



			mesh.Draw(shaderProgram, camera);

			gui.Begin();

			ImGui::SetNextWindowPos(ImVec2(1620, 0), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(300, 1080), ImGuiCond_Always);

			ImGui::Begin("Main UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMove);
			ImGui::Text("Cube Transform");


			ImGui::InputFloat3("Position", glm::value_ptr(meshes[0].position));
			ImGui::InputFloat3("Rotation", glm::value_ptr(meshes[0].rotation));

			static bool uniformScaleLock = true;
			static float uniformScale = meshes[0].scale.x;  // Initial uniform scale

			ImGui::Checkbox("Uniform Scale", &uniformScaleLock);
			ImGui::Text("Mouse X: %.2f", mouseX);
			ImGui::Text("Mouse Y: %.2f", mouseY);
			ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);


			if (uniformScaleLock) {
				if (ImGui::InputFloat("Scale", &uniformScale, 0.1f)) {
					meshes[0].scale = glm::vec3(uniformScale);
				}
			} else {
				ImGui::InputFloat3("Scale", glm::value_ptr(meshes[0].scale));
			}

			if (ImGui::Button("Apply Transform")) {
				meshes[0].ApplyTransformations(); // Apply on button click
			}

			ImGui::End();

			gui.End();
		}





		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}

	gui.CleanUp();

	shaderProgram.Delete();
	//Delete window before ending the program
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();

	return 0;
}