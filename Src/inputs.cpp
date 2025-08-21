#include"../include/inputs.h"

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

void Inputs::MeshInputs(GLFWwindow* window, const std::vector<std::unique_ptr<Mesh>>& meshes, int &currentMesh, int &selectedMeshType) {

    if (isDown(GLFW_KEY_UP, false, window)) {
        (meshes[currentMesh].get()->*currentTransform).z -= 0.1;
    }
    if (isDown(GLFW_KEY_DOWN, false, window)) {
        (meshes[currentMesh].get()->*currentTransform).z += 0.1;
    }
    if (isDown(GLFW_KEY_RIGHT, false, window)) {
        (meshes[currentMesh].get()->*currentTransform).x += 0.1;
    }
    if (isDown(GLFW_KEY_LEFT, false, window)) {
        (meshes[currentMesh].get()->*currentTransform).x -= 0.1;
    }
    if (isDown(GLFW_KEY_PAGE_UP, false, window)) {
        (meshes[currentMesh].get()->*currentTransform).y += 0.1;
    }
    if (isDown(GLFW_KEY_PAGE_DOWN, false, window)) {
        (meshes[currentMesh].get()->*currentTransform).y -= 0.1;
    }
    if (isDown(GLFW_KEY_RIGHT_BRACKET, true, window)) {
        currentMesh++;
    }
    if (isDown(GLFW_KEY_LEFT_BRACKET, true, window)) {
        currentMesh--;
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

void Inputs::InputHandler(GLFWwindow* window, glm::vec3 &lightPos, const std::vector<std::unique_ptr<Mesh>>& meshes, int &currentMesh, int &selectedMeshType) {
    if (isDown(GLFW_KEY_M, true, window)) {
        currentMode = 0;
    }
    if (isDown(GLFW_KEY_L, true, window)) {
        currentMode = 1;
    }

    if (currentMode == 0) {
        MeshInputs(window, meshes, currentMesh, selectedMeshType);
    }
    else if (currentMode == 1) {
        LightInputs(lightPos, window);
    }
}


