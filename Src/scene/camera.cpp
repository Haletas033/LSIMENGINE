#include "../../include/scene/camera.h"

#include "engineContext.h"
#include "utils/defaults.h"
#include "vk/vk.h"

std::expected<Camera, LSIM::Error> Camera::create(const Device& device, int width, int height, glm::vec3 position) {
        Camera camera;
        camera.width = width;
        camera.height = height;
        camera.position = position;
        camera.FOV = EngineContext::getDefaults().FOVdeg;
        camera.near = EngineContext::getDefaults().nearPlane;
        camera.far = EngineContext::getDefaults().farPlane;

        for (auto& uniformBuffer : camera.perFrameUBOs) {
                auto buffer = Buffer::create(
                        device,
                        sizeof(CameraUBO),
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                );

                if (!buffer)
                        return std::unexpected(buffer.error());

                uniformBuffer = std::move(*buffer);
        }

        DescriptorSet::BindingDescriptor binding{
                .binding = 0,
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        };

        std::array bindings = {binding};

        VkDescriptorBufferInfo bufferInfo0{
                .buffer = camera.perFrameUBOs[0].getBuffer(),
                .offset = 0,
                .range = sizeof(CameraUBO)
        };

        VkDescriptorBufferInfo bufferInfo1{
                .buffer = camera.perFrameUBOs[1].getBuffer(),
                .offset = 0,
                .range = sizeof(CameraUBO)
        };

        DescriptorSet::WriteDescriptor write0{
                .binding = 0,
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .info = bufferInfo0
        };

        DescriptorSet::WriteDescriptor write1{
                .binding = 0,
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .info = bufferInfo1
        };

        std::array frame0Writes{write0};
        std::array frame1Writes{write1};

        std::array<std::span<const DescriptorSet::WriteDescriptor>, 2> writes{
                std::span{frame0Writes},
                std::span{frame1Writes}
        };

        auto descriptorSet = DescriptorSet::create(
                device.getLogicalDevice(),
                bindings,
                writes
        );

        if (!descriptorSet)
                return std::unexpected(descriptorSet.error());

        camera.descriptorSet = std::move(*descriptorSet);
        return camera;
}

float pitch = 0.0f;
float yaw = -90.0f;
float speed = 1.0f;

std::expected<void, LSIM::Error> Camera::update(const uint32_t frameIndex) {
        UBO.view = glm::lookAt(position, position + orientation, up);
        UBO.projection = glm::perspective(glm::radians(FOV), static_cast<float>(width) / static_cast<float>(height), near, far);
        UBO.projection[1][1] *= -1.0f;
        if (auto res = perFrameUBOs[frameIndex].upload(&UBO); !res) {
                return std::unexpected(res.error());
        }
        return {};
}

void Camera::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
        Defaults defaults = EngineContext::getDefaults();
        if (yOffset > 0) {
                defaults.speedMultiplier += 1.0f;
        }
        else if (yOffset < 0) {
                defaults.speedMultiplier -= 1.0f;
        }

        if (defaults.speedMultiplier < 0) {
                defaults.speedMultiplier = 0.01;
        }
}


void Camera::Inputs(GLFWwindow* window, float deltaTime) {
        Defaults defaults = EngineContext::getDefaults();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                position += speed * orientation * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                position -= speed * orientation * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                position -= speed * glm::normalize(glm::cross(orientation, up)) * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                position += speed * glm::normalize(glm::cross(orientation, up)) * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                position += speed * up * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                position -= speed * up * deltaTime;

        //Fast mode
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                speed = 0.8f * defaults.speedMultiplier;
        else
                speed = 0.4f * defaults.speedMultiplier;

        //Handle mouse input for looking around
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                if (firstMouseMove) {
                        lastMouseX = mouseX;
                        lastMouseY = mouseY;
                        firstMouseMove = false;
                }

                //Calculate mouse deltas
                const double deltaX = mouseX - lastMouseX; // Mouse movement in X (horizontal)
                const double deltaY = mouseY - lastMouseY; // Mouse movement in Y (vertical)

                lastMouseX = mouseX;
                lastMouseY = mouseY;

                //Apply sensitivity to the delta values
                pitch -= defaults.sensitivity * static_cast<float>(deltaY) / static_cast<float>(height);
                //Vertical (pitch) rotation
                yaw += defaults.sensitivity * static_cast<float>(deltaX) / static_cast<float>(width);
                //Horizontal (yaw) rotation

                //Clamps pitch to avoid flipping
                if (pitch > 89.0f) pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;

                //Rebuilds the orientation vector
                glm::vec3 direction;
                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                orientation = glm::normalize(direction);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                firstMouseMove = true;
        }

        glfwSetScrollCallback(window, scroll_callback);
}

Camera::CameraDirection Camera::getDirection() const {
        CameraDirection cameraDirection = {};

        const glm::vec2 flatForward(orientation.x, orientation.z);
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
                cameraDirection.forward = glm::vec3(-1, 0, 0);
                cameraDirection.side = glm::vec3(0, 0, 1);
        }
        else if (yawDeg > 45 && yawDeg <= 135) {
                // Facing backwards relative to world view
                cameraDirection.forward = glm::vec3(0, 0, -1);
                cameraDirection.side = glm::vec3(-1, 0, 0);
        }
        else if (yawDeg > 135 || yawDeg <= -135) {
                // Facing right relative to world view
                cameraDirection.forward = glm::vec3(1, 0, 0);
                cameraDirection.side = glm::vec3(0, 0, -1);
        }
        else {
                // Facing forward relative to world view
                cameraDirection.forward = glm::vec3(0, 0, 1);
                cameraDirection.side = glm::vec3(1, 0, 0);
        }

        return cameraDirection;
}
