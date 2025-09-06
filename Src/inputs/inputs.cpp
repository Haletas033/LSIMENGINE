#include "../../include/inputs/inputs.h"

#include <algorithm>
#include <iostream>

bool Inputs::isDown(const int key, const bool onlyOnPress, GLFWwindow* window) {

    if (canPress.find(key) == canPress.end()) {
        canPress[key] = true;
    }

    if (glfwGetKey(window, key) == GLFW_PRESS) {
        if (onlyOnPress) {
            if (canPress[key]) {
                canPress[key] = false;
                return true;
            }
            return false;
        }
        return true;

    }

    if (glfwGetKey(window, key) == GLFW_RELEASE) {
        canPress[key] = true;
        return false;
    }
    return false;
}

void Inputs::MeshInputs(GLFWwindow* window, const Scene &scene,
    const int &currentMesh, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation) {
    // Flatten y
    Orientation.y = 0;

    float *cameraForward = nullptr;
    float *cameraSide = nullptr;
    bool positive;

    const glm::vec2 flatForward(Orientation.x, Orientation.z);
    const double yaw = atan2(flatForward.y, flatForward.x);
    double yawDeg = yaw * 180.0f / glm::pi<float>();

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
        cameraForward = &(scene.meshes[currentMesh].get()->*currentTransform).x;
        cameraSide = &(scene.meshes[currentMesh].get()->*currentTransform).z;
        positive = false;
    }
    else if (yawDeg > 45 && yawDeg <= 135) {
        // Facing backwards relative to world view
        cameraForward = &(scene.meshes[currentMesh].get()->*currentTransform).z;
        cameraSide = &(scene.meshes[currentMesh].get()->*currentTransform).x;
        positive = false;
    }
    else if (yawDeg > 135 || yawDeg <= -135) {
        // Facing right relative to world view
        cameraForward = &(scene.meshes[currentMesh].get()->*currentTransform).x;
        cameraSide = &(scene.meshes[currentMesh].get()->*currentTransform).z;
        positive = true;
    }
    else {
        // Facing forward relative to world view
        cameraForward = &(scene.meshes[currentMesh].get()->*currentTransform).z;
        cameraSide = &(scene.meshes[currentMesh].get()->*currentTransform).x;
        positive = true;
    }

    float &cameraUp = (scene.meshes[currentMesh].get()->*currentTransform).y;

    if (isDown(GLFW_KEY_UP, false, window)) {
        *cameraForward -= positive ? 0.1 : -0.1;
    }
    if (isDown(GLFW_KEY_DOWN, false, window)) {
        *cameraForward += positive ? 0.1 : -0.1;
    }
    if (isDown(GLFW_KEY_RIGHT, false, window)) {
         *cameraSide += positive ? 0.1 : -0.1;
    }
    if (isDown(GLFW_KEY_LEFT, false, window)) {
        *cameraSide -= positive ? 0.1 : -0.1;
    }
    if (isDown(GLFW_KEY_PAGE_UP, false, window)) {
         cameraUp += 0.1;
    }
    if (isDown(GLFW_KEY_PAGE_DOWN, false, window)) {
        cameraUp -= 0.1;
    }
    if (isDown(GLFW_KEY_RIGHT_BRACKET, true, window)) {
        selectedMesh++;
        selectedMesh = std::clamp(selectedMesh, 0, static_cast<int>(scene.meshes.size() - 1));
    }
    if (isDown(GLFW_KEY_LEFT_BRACKET, true, window)) {
        selectedMesh--;
        selectedMesh = std::clamp(selectedMesh, 0, static_cast<int>(scene.meshes.size() - 1));
    }

    if (isDown(GLFW_KEY_G, true, window)) {
        currentTransform = &Mesh::position;
    }
    if (isDown(GLFW_KEY_R, true, window)) {
        currentTransform = &Mesh::rotation;
    }
    if (isDown(GLFW_KEY_N, true, window)) {
        currentTransform = &Mesh::scale;
    }

    for (int i = 0 + GLFW_KEY_0; i < 10 + GLFW_KEY_0; i++) {
        if (isDown(i, true, window)) {
            selectedMeshType = i - 48;
        }
    }
}

void Inputs::LightInputs(Scene &scene, const int &currentLight, GLFWwindow* window) {
    if (isDown(GLFW_KEY_UP, false, window)) {
        scene.lights[currentLight].lightPos.z -= 0.1;
    }
    if (isDown(GLFW_KEY_DOWN, false, window)) {
        scene.lights[currentLight].lightPos.z += 0.1;
    }
    if (isDown(GLFW_KEY_RIGHT, false, window)) {
        scene.lights[currentLight].lightPos.x += 0.1;
    }
    if (isDown(GLFW_KEY_LEFT, false, window)) {
        scene.lights[currentLight].lightPos.x -= 0.1;
    }
    if (isDown(GLFW_KEY_PAGE_UP, false, window)) {
        scene.lights[currentLight].lightPos.y += 0.1;
    }
    if (isDown(GLFW_KEY_PAGE_DOWN, false, window)) {
        scene.lights[currentLight].lightPos.y -= 0.1;
    }

}

void Inputs::IOInputs(GLFWwindow *window, Scene &scene) {
    if (isDown(GLFW_KEY_O, true, window)) {
        std::cout << "Saving" << std::endl;
        std::string fileName = IO::Dialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0", GetSaveFileNameA);
        if (std::ofstream file(fileName, std::ios::out | std::ios::binary); file.is_open()) {
            IO::saveToFile(file, scene);
        }
        std::cout << "Saved" << std::endl;
    }

    if (isDown(GLFW_KEY_I, true, window)) {
        std::cout << "Loading" << std::endl;
        std::string fileName = IO::Dialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0", GetOpenFileNameA);
        if (std::ifstream file(fileName, std::ios::in | std::ios::binary); file.is_open()) {
            scene = IO::loadFromFile(file);
        }
        std::cout << "Loaded" << std::endl;
    }
}

void Inputs::InputHandler(GLFWwindow* window, Scene &scene,
    const int &currentMesh, const int &currentLight, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation) {
    if (isDown(GLFW_KEY_M, true, window)) {
        currentMode = meshMode;
    }
    if (isDown(GLFW_KEY_L, true, window)) {
        currentMode = lightMode;
    }

    if (currentMode == meshMode) {
        if (!scene.meshes.empty() && currentMesh >= 0 && currentMesh < scene.meshes.size()) {
            MeshInputs(window, scene, currentMesh, selectedMeshType, selectedMesh, Orientation);
        }

        // Handle adding and deleting meshes
        if (isDown(GLFW_KEY_F, true, window)) {
            scene.addMeshSignal = true;
        }
        if (!scene.meshes.empty() && isDown(GLFW_KEY_DELETE, true, window)) {
            scene.deleteMeshSignal = true;
        }
    }
    else if (currentMode == lightMode) {
        LightInputs(scene, currentLight, window);

        //Handle adding and deleting lights
        if (isDown(GLFW_KEY_F, true, window)) {
            scene.addLightSignal = true;
        }
        if (!scene.lights.empty() && isDown(GLFW_KEY_DELETE, true, window)) {
            scene.deleteLightSignal = true;
        }
    }

    IOInputs(window, scene);
}


