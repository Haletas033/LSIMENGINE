#include "../include/camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	Position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform, float aspect) const
{
	glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

float pitch = 0.0f;
float yaw = -90.0f;

float speedMultiplier = 50.0f;

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    if (yOffset > 0) {
        speedMultiplier += 1.0f;
    }
    else if (yOffset < 0) {
        speedMultiplier -= 1.0f;
    }

    if (speedMultiplier < 0) {
        speedMultiplier = 0.01;
    }
}


void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += speed * Orientation * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= speed * Orientation * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= speed * glm::normalize(glm::cross(Orientation, Up)) * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += speed * glm::normalize(glm::cross(Orientation, Up)) * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Position += speed * Up * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        Position -= speed * Up * deltaTime;

    //Fast mode
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed = 0.8f * speedMultiplier;
    else
        speed = 0.4f * speedMultiplier;

    //Handle mouse input for looking around
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if (firstMouseMove)
        {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouseMove = false;
        }

        //Calculate mouse deltas
        double deltaX = mouseX - lastMouseX;// Mouse movement in X (horizontal)
        double deltaY = mouseY - lastMouseY;// Mouse movement in Y (vertical)

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        //Apply sensitivity to the delta values
        pitch -= sensitivity * static_cast<float>(deltaY) / static_cast<float>(height); //Vertical (pitch) rotation
        yaw += sensitivity * static_cast<float>(deltaX) / static_cast<float>(width);  //Horizontal (yaw) rotation

        //Clamps pitch to avoid flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

            //Rebuilds the orientation vector
            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            Orientation = glm::normalize(direction);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouseMove = true;
    }

    glfwSetScrollCallback(window, scroll_callback);
}


