#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#define GLM_ENABLE_EXPERIMENTAL
#define GLFW_INCLUDE_NONE
#include <expected>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "LSIMtypes.h"
#include "vk/buffer.h"
#include "vk/descriptorSet.h"
#include "vk/device.h"

struct CameraUBO {
        glm::mat4 view{};
        glm::mat4 projection{};
};

class Camera {
private:
        glm::vec3 position{};
        glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        bool firstMouseMove = true;

        int width{};
        int height{};

        double lastMouseX = 0.0;
        double lastMouseY = 0.0;

        float FOV = 90.f;
        float near = 0.f;
        float far = 1000.f;

        CameraUBO UBO;

        std::array<Buffer, 2> perFrameUBOs{};
        DescriptorSet descriptorSet{};

public:
        struct CameraDirection {
                glm::vec3 forward, side;
        };

        Camera() = default;

        static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

        static std::expected<Camera, LSIM::Error> create(const Device&, int width, int height, glm::vec3 position);
        std::expected<void, LSIM::Error> update(uint32_t frameIndex);
        [[nodiscard]] const DescriptorSet& getDescriptorSet() const { return descriptorSet; }

        void Inputs(GLFWwindow* window, float deltaTime);

        [[nodiscard]] CameraDirection getDirection() const;
};

#endif
