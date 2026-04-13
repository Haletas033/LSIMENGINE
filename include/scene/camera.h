#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#define GLM_ENABLE_EXPERIMENTAL
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "../gl/shaderClass.h"
#include "include/utils/defaults.h"

class Camera
{
public:
    //Stores the main vectors of the camera
    glm::vec3 Position{};
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    //Prevents the camera from jumping around when first clicking left click
    bool firstMouseMove = true;

    //Stores the width and height of the window
    int width;
    int height;

    //Variables to store mouse position for delta calculation
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    glm::mat4 view;
    glm::mat4 projection;

    //Default constructor
    Camera() = default;

    //Camera constructor to set up initial values
    Camera(int width, int height, glm::vec3 position);

    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

    //Updates and exports the camera matrix to the Vertex Shader
    void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform, float aspect);

    //Handles camera inputs (keyboard and mouse)
    void Inputs(GLFWwindow* window, float deltaTime);


};

#endif
