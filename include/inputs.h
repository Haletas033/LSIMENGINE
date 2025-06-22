#ifndef INPUTS_CLASS_H
#define INPUTS_CLASS_H

#include"../include/mesh.h"

#include<unordered_map>
#include<GLFW/glfw3.h>
#include<vector>

class Inputs {
public:
    std::unordered_map<int, bool> canPress;

    glm::vec3 Mesh::*currentTransform = &Mesh::position;

    int currentMode = 0;

    bool isDown(int key, bool onlyOnPress, GLFWwindow* window);

    void MeshInputs(GLFWwindow* window, std::vector<Mesh> &meshes, int &currentMesh, int &selectedMeshType);

    void LightInputs(glm::vec3 &lightPos, GLFWwindow* window);

    void InputHandler(GLFWwindow* window, glm::vec3 &lightPos, std::vector<Mesh> &meshes, int &currentMesh, int &selectedMeshType);
};

#endif //INPUTS_CLASS_H
