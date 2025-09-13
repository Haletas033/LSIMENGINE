#ifndef INPUTS_CLASS_H
#define INPUTS_CLASS_H

#include <memory>

#include "../scene/scene.h"
#include "../utils/fileIO.h"

#include<unordered_map>
#include<GLFW/glfw3.h>
#include<vector>

#include "include/utils/defaults.h"

class Inputs {
private:
    bool isDown(int key, bool onlyOnPress, GLFWwindow* window);

    void MeshInputs(GLFWwindow* window, const Scene &scene, const int &currentMesh, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation);

    void LightInputs(Scene &scene, const int &currentLight, GLFWwindow* window);

    void IOInputs(GLFWwindow* window, Scene &scene);
public:
    std::unordered_map<int, bool> canPress;

    glm::vec3 Mesh::*currentTransform = &Mesh::position;

    Defaults defaults;

    enum CurrentMode {
        meshMode,
        lightMode
    };

    CurrentMode currentMode = meshMode;

    static void InitInputs();

    void InputHandler(GLFWwindow* window, Scene &scene,
    const int &currentMesh, const int &currentLight, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation);
};

#endif //INPUTS_CLASS_H
