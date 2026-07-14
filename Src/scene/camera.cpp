#include "../../include/scene/camera.h"

extern Defaults engineDefaults;

Camera::Camera(int width, int height, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	Position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform, float aspect)
{
	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);

	glUniformMatrix4fv(shader.GetLocation(uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

float pitch = 0.0f;
float yaw = -90.0f;
float speed = 1.0f;

void Camera::scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    if (yOffset > 0) {
        engineDefaults.speedMultiplier += 1.0f;
    }
    else if (yOffset < 0) {
        engineDefaults.speedMultiplier -= 1.0f;
    }

    if (engineDefaults.speedMultiplier < 0) {
        engineDefaults.speedMultiplier = 0.01;
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
        speed = 0.8f * engineDefaults.speedMultiplier;
    else
        speed = 0.4f * engineDefaults.speedMultiplier;

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
        const double deltaX = mouseX - lastMouseX;// Mouse movement in X (horizontal)
        const double deltaY = mouseY - lastMouseY;// Mouse movement in Y (vertical)

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        //Apply sensitivity to the delta values
        pitch -= engineDefaults.sensitivity * static_cast<float>(deltaY) / static_cast<float>(height); //Vertical (pitch) rotation
        yaw += engineDefaults.sensitivity * static_cast<float>(deltaX) / static_cast<float>(width);  //Horizontal (yaw) rotation

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

Camera::CameraDirection Camera::getDirection() const {
    CameraDirection cameraDirection = {};

    const glm::vec2 flatForward(Orientation.x, Orientation.z);
    const double facingAngle = atan2(flatForward.y, flatForward.x);
    double yawDeg = facingAngle * 180.0f / glm::pi<float>();

    /*  Relative to the world

              forward
                -90d
                 |
       left 0d --+-- 180d right
                 |
                90d
             backwards
    */

    if (yawDeg >= -45 && yawDeg <= 45) {
        // Facing left relative to word view
        cameraDirection.forward = glm::vec3(-1,0,0);
        cameraDirection.side = glm::vec3(0,0,1);
    }
    else if (yawDeg > 45 && yawDeg <= 135) {
        // Facing backwards relative to world view
        cameraDirection.forward = glm::vec3(0,0,-1);
        cameraDirection.side = glm::vec3(-1,0,0);
    }
    else if (yawDeg > 135 || yawDeg <= -135) {
        // Facing right relative to world view
        cameraDirection.forward = glm::vec3(1,0,0);
        cameraDirection.side = glm::vec3(0,0,-1);
    }
    else {
        // Facing forward relative to world view
        cameraDirection.forward = glm::vec3(0,0,1);
        cameraDirection.side = glm::vec3(1,0,0);
    }

    return cameraDirection;
}


