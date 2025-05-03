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



const unsigned int width = 800;
const unsigned int height = 800;

//Callback function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};


int main()
{
	//Initializes GLFW
	glfwInit();

	//Tells GLFW what version of OpenGL we are using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Tells GLFW we are using the CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	//Creates a GLFWwindow object of 800 by 800 pixels
	GLFWwindow* window = glfwCreateWindow(width, height, "LSIMENGINE", NULL, NULL);

	//Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//Introduces the window into the current context
	glfwMakeContextCurrent(window);

	//Loads GLAD so it configures OpenGL
	gladLoadGL();
	//Specifies the viewport of OpenGL in the Window
	glViewport(0, 0, width, height);

	// Register the window resize callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("../shaders/default.vert", "../shaders/default.frag");


	//Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	//Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	//Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	//Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//Unbinds all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();



	//Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	//Main while loop
	while (!glfwWindowShouldClose(window))
	{
		//Specifies the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//Cleans the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Tells OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		//Handles camera inputs
		camera.Inputs(window);

		// Update aspect ratio from current framebuffer size
		int windowWidth, windowHeight;

		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		float aspect = (float)windowWidth / (float)windowHeight;

		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix", aspect);

;
		//Binds the VAO so OpenGL knows to use it
		VAO1.Bind();
		//Draws primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//Swaps the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Takes care of all GLFW events
		glfwPollEvents();
	}



	//Deletes all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();
	//Deletes window before ending the program
	glfwDestroyWindow(window);
	//Terminates GLFW before ending the program
	glfwTerminate();
	return 0;
}