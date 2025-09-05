#ifndef INPUTS_CLASS_H
#define INPUTS_CLASS_H

#include <memory>

#include"scene.h"
#include"fileIO.h"

#include<unordered_map>
#include<GLFW/glfw3.h>
#include<vector>

class Inputs {
public:
    std::unordered_map<int, bool> canPress;

    glm::vec3 Mesh::*currentTransform = &Mesh::position;

    int currentMode = 0;

    bool isDown(int key, bool onlyOnPress, GLFWwindow* window);

    void MeshInputs(GLFWwindow* window, Scene &scene, int &currentMesh, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation);

    void LightInputs(Scene &scene, const int &currentLight, GLFWwindow* window);

    void InputHandler(GLFWwindow* window, Scene &scene,
    int &currentMesh, const int &currentLight, int &selectedMeshType, int &selectedMesh, glm::vec3 Orientation);
};

#endif //INPUTS_CLASS_H
