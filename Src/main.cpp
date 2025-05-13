#include<iostream>
#include"../include/terrain.h"
#include"../include/mesh.h"
#include"../include/primitives.h"


const unsigned int width = 800;
const unsigned int height = 800;

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

	std::vector<std::vector<float>> noiseMap = GenerateNoiseMap(1024, 1024, static_cast<unsigned int>(time(nullptr)), 15.0f, 8, 0.5f, 2.0f);

	GLuint noiseMapTexture = noiseMapToTexture(noiseMap);

	noiseMapToMesh(noiseMap, vertices, indices, 80, 2.0f);

	primitives p;


	Mesh terrain(vertices, indices);

	Mesh cube = p.GenerateCube();

	meshes.push_back(terrain);
	meshes.push_back(cube);

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
		camera.Inputs(window);

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

			glm::vec3 translation(0.0f);
			float rotationAngle = 0.0f;
			glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);
			glm::vec3 scale(1.0f);

			if (i == 0) {
				translation = glm::vec3(1.0f, 0.0f, 0.0f);
				rotationAngle = 45.0f;
				scale = glm::vec3(1.0f);
			}
			else if (i == 1) {
				translation = glm::vec3(-1.0f, 0.0f, 0.0f);
				rotationAngle = 90.0f;
				scale = glm::vec3(0.5f);
			}

			mesh.ApplyTransformations(translation, rotationAngle, rotationAxis, scale);
			mesh.Draw(shaderProgram, camera);
		}


		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}


	shaderProgram.Delete();
	//Delete window before ending the program
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();

	return 0;
}