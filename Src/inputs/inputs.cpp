#include <include/inputs/inputs.h>
#include <include/utils/logging/log.h>

#include <algorithm>
#include <iostream>

#include "include/utils/json.h"

extern json config;

extern std::vector<Logger> logs;

static std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;

static void Log(const std::string& key, const std::string& msg) {
    if (const auto it = loggers.find(key); it != loggers.end())
        (*it->second)(msg, logs);
}

void Inputs::InitInputs() {
    JSONManager::LoadLoggers(config, loggers);

    // Ensure logger exists
    if (!loggers.count("stdInfo"))
        loggers["stdInfo"] = std::make_unique<Logger>();

    loggers["stdInfo"]->SetModule("INPUTS");
    loggers["stdWarn"]->SetModule("INPUTS");
    loggers["stdError"]->SetModule("INPUTS");

    Log("stdInfo", "Successfully initialized the input loggers");
}

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
        *cameraForward -= positive ? defaults.transformSpeed : -defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_DOWN, false, window)) {
        *cameraForward += positive ? defaults.transformSpeed : -defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_RIGHT, false, window)) {
         *cameraSide += positive ? defaults.transformSpeed : -defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_LEFT, false, window)) {
        *cameraSide -= positive ? defaults.transformSpeed : -defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_PAGE_UP, false, window)) {
         cameraUp += defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_PAGE_DOWN, false, window)) {
        cameraUp -= defaults.transformSpeed;
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
        Log("stdInfo", "Set position as the current transform");
    }
    if (isDown(GLFW_KEY_R, true, window)) {
        currentTransform = &Mesh::rotation;
        Log("stdInfo", "Set Rotation as the current transform");
    }
    if (isDown(GLFW_KEY_N, true, window)) {
        currentTransform = &Mesh::scale;
        Log("stdInfo", "Set scale as the current transform");
    }

    for (int i = 0 + GLFW_KEY_0; i < 10 + GLFW_KEY_0; i++) {
        if (isDown(i, true, window)) {
            selectedMeshType = i - 48;
        }
    }
}

void Inputs::LightInputs(Scene &scene, const int &currentLight, GLFWwindow* window) {
    if (isDown(GLFW_KEY_UP, false, window)) {
        scene.lights[currentLight].lightPos.z -= defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_DOWN, false, window)) {
        scene.lights[currentLight].lightPos.z += defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_RIGHT, false, window)) {
        scene.lights[currentLight].lightPos.x += defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_LEFT, false, window)) {
        scene.lights[currentLight].lightPos.x -= defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_PAGE_UP, false, window)) {
        scene.lights[currentLight].lightPos.y += defaults.transformSpeed;
    }
    if (isDown(GLFW_KEY_PAGE_DOWN, false, window)) {
        scene.lights[currentLight].lightPos.y -= defaults.transformSpeed;
    }

}

void Inputs::IOInputs(GLFWwindow *window, Scene &scene, const std::string & workingDir) {
    if (isDown(GLFW_KEY_O, true, window)) {
        std::string fileName = IO::Dialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0", GetSaveFileNameA);
        if (std::ofstream file(fileName, std::ios::out | std::ios::binary); file.is_open()) {
            IO::saveToFile(file, scene);
        }
    }

    if (isDown(GLFW_KEY_I, true, window)) {
        std::string fileName = IO::Dialog("LSIM Files\0*.lsim\0All Files\0*.*\0\0", GetOpenFileNameA);
        if (std::ifstream file(fileName, std::ios::in | std::ios::binary); file.is_open()) {
            scene = IO::loadFromFile(file, workingDir);
        }
    }
}

void Inputs::InputHandler(GLFWwindow* window, Scene &scene, const std::string &workingDir,
    const int &currentMesh, const int &currentLight, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation) {
    if (isDown(GLFW_KEY_M, true, window)) {
        currentMode = meshMode;
        Log("stdInfo", "Switched to mesh mode");
    }
    if (isDown(GLFW_KEY_L, true, window)) {
        currentMode = lightMode;
        Log("stdInfo", "Switched to light mode");
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
        } else if (scene.meshes.empty() && isDown(GLFW_KEY_DELETE, true, window)) {
            Log("stdWarn", "Pressed delete on mesh but there were none left");
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
        } else if (scene.lights.empty() && isDown(GLFW_KEY_DELETE, true, window)) {
            Log("stdWarn", "Pressed delete for light but there were none left");
        }
    }

    IOInputs(window, scene, workingDir);
}


