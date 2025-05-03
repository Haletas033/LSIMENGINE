#include "../include/camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	Position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform, float aspect)
{
	glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
	glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* window)
{
    //Handle movement keys (W, A, S, D, Space, Left Control, Left Shift)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += speed * Orientation;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= speed * Orientation;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= speed * glm::normalize(glm::cross(Orientation, Up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += speed * glm::normalize(glm::cross(Orientation, Up));
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Position += speed * Up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        Position -= speed * Up;

    //Fast mode
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed = 0.4f;
    else
        speed = 0.1f;

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
        float rotX = sensitivity * (float)deltaY / height; //Vertical (pitch) rotation
        float rotY = sensitivity * (float)deltaX / width;  //Horizontal (yaw) rotation

        //Vertical rotation (pitch) - looking up and down
        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

        //Clamp the pitch to prevent flipping the camera upside down
        if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            Orientation = newOrientation;
        }

        //Horizontal rotation (yaw) - looking left and right
        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouseMove = true;
    }
}
