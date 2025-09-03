#include"../include/inputs.h"

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

void Inputs::MeshInputs(GLFWwindow* window, std::vector<std::unique_ptr<Mesh>>& meshes,
    int &currentMesh, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation) {
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
        cameraForward = &(meshes[currentMesh].get()->*currentTransform).x;
        cameraSide = &(meshes[currentMesh].get()->*currentTransform).z;
        positive = false;
    }
    else if (yawDeg > 45 && yawDeg <= 135) {
        // Facing backwards relative to world view
        cameraForward = &(meshes[currentMesh].get()->*currentTransform).z;
        cameraSide = &(meshes[currentMesh].get()->*currentTransform).x;
        positive = false;
    }
    else if (yawDeg > 135 || yawDeg <= -135) {
        // Facing right relative to world view
        cameraForward = &(meshes[currentMesh].get()->*currentTransform).x;
        cameraSide = &(meshes[currentMesh].get()->*currentTransform).z;
        positive = true;
    }
    else {
        // Facing forward relative to world view
        cameraForward = &(meshes[currentMesh].get()->*currentTransform).z;
        cameraSide = &(meshes[currentMesh].get()->*currentTransform).x;
        positive = true;
    }

    float &cameraUp = (meshes[currentMesh].get()->*currentTransform).y;

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
    }
    if (isDown(GLFW_KEY_LEFT_BRACKET, true, window)) {
        selectedMesh--;
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

    if (isDown(GLFW_KEY_O, true, window)) {
        std::cout << "Saving" << std::endl;
        std::string fileName = IO::Dialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0", GetSaveFileNameA);
        if (std::ofstream file(fileName, std::ios::out | std::ios::binary); file.is_open()) {
            IO::saveToFile(file, meshes);
        }
        std::cout << "Saved" << std::endl;
    }

    if (isDown(GLFW_KEY_I, true, window)) {
        std::cout << "Loading" << std::endl;
        std::string fileName = IO::Dialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0", GetOpenFileNameA);
        if (std::ifstream file(fileName, std::ios::in | std::ios::binary); file.is_open()) {
            meshes = IO::loadFromFile(file);
        }
        std::cout << "Loaded" << std::endl;
    }

    for (int i = 0 + GLFW_KEY_0; i < 10 + GLFW_KEY_0; i++) {
        if (isDown(i, true, window)) {
            selectedMeshType = i - 48;
        }
    }
}

void Inputs::LightInputs(glm::vec3 &lightPos, GLFWwindow* window) {
    if (isDown(GLFW_KEY_UP, false, window)) {
        lightPos.z -= 0.1;
    }
    if (isDown(GLFW_KEY_DOWN, false, window)) {
        lightPos.z += 0.1;
    }
    if (isDown(GLFW_KEY_RIGHT, false, window)) {
        lightPos.x += 0.1;
    }
    if (isDown(GLFW_KEY_LEFT, false, window)) {
        lightPos.x -= 0.1;
    }
    if (isDown(GLFW_KEY_PAGE_UP, false, window)) {
        lightPos.y += 0.1;
    }
    if (isDown(GLFW_KEY_PAGE_DOWN, false, window)) {
        lightPos.y -= 0.1;
    }
}

void Inputs::InputHandler(GLFWwindow* window, glm::vec3 &lightPos, std::vector<std::unique_ptr<Mesh>>& meshes,
    int &currentMesh, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation) {
    if (isDown(GLFW_KEY_M, true, window)) {
        currentMode = 0;
    }
    if (isDown(GLFW_KEY_L, true, window)) {
        currentMode = 1;
    }

    if (currentMode == 0) {
        if (!meshes.empty() && currentMesh >= 0 && currentMesh < meshes.size()) {
            MeshInputs(window, meshes, currentMesh, selectedMeshType, selectedMesh, Orientation);
        }
    }
    else if (currentMode == 1) {
        LightInputs(lightPos, window);
    }
}


