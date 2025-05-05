#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "../include/shaderClass.h"
#include "../include/VAO.h"
#include"../include/VBO.h"
#include "../include/EBO.h"
#include"../include/camera.h"
#include"../include/terrain.h"



const unsigned int width = 800;
const unsigned int height = 800;

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

//Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.0f, -0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
	-0.5,  0.0f,  0.5f,    1.0f, 0.0f, 1.0f,    0.0f, 1.0f
};

//Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};


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


	//Generate Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	//Generate Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	//Generate Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	//Link VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	//Enable the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Create camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	std::vector<std::vector<float>> noiseMap = GenerateNoiseMap(512, 512, static_cast<unsigned int>(time(nullptr)), 15.0f, 8, 0.5f, 2.0f);

	GLuint noiseMapTexture = noiseMapToTexture(noiseMap);



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

		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix", aspect);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, noiseMapTexture);
		//Set the sampler uniform to use texture unit 0
		GLint texLoc = glGetUniformLocation(shaderProgram.ID, "tex0");
		glUniform1i(texLoc, 0);

		//Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		//Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Take care of all GLFW events
		glfwPollEvents();
	}



	//Deletes all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	//Delete window before ending the program
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();

	return 0;
}