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
    void MeshInputs(::GLFWwindow *window, const ::Scene &scene, float deltaTime, const int &currentMesh, int &selectedMesh, glm::vec3
                    Orientation);

    void LightInputs(Scene &scene, float deltaTime, const int &currentLight, GLFWwindow *window);

    void IOInputs(GLFWwindow *window, Scene &scene, const std::string &workingDir);
public:
    std::unordered_map<int, bool> canPress;

    glm::vec3 Mesh::*currentTransform = &Mesh::position;

    Defaults defaults;

    enum CurrentMode {
        meshMode,
        lightMode
    };

    CurrentMode currentMode = meshMode;

    bool isDown(int key, bool onlyOnPress, GLFWwindow* window);

    static void InitInputs();

    void InputHandler(::GLFWwindow *window, ::Scene &scene, float deltaTime,
                      const ::std::string &workingDir, const int &currentMesh, const int &currentLight, int &selectedMeshType, int &
                      selectedMesh, glm::vec3 Orientation);
};

#endif //INPUTS_CLASS_H
