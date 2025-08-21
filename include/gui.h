#ifndef GUI_CLASS_H
#define GUI_CLASS_H

#include <memory>
#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"../imgui/imgui.h"
#include"../imgui/backends/imgui_impl_glfw.h"
#include"../imgui/backends/imgui_impl_opengl3.h"

#include"../include/mesh.h"

class Gui {
    public:
    struct Node {
        Mesh* mesh;
        Node* parent;
        std::vector<Node*> children;
    };

    static Node* root;

    static void Initialize(GLFWwindow* window);

    static void Begin();

    static void End();

    static void CleanUp();

    static void Transform(const std::vector<std::unique_ptr<Mesh>> &meshes, std::vector<int> &currentMeshes, int &selectedMeshType, int clickedMesh);

    static void Lighting(glm::vec4 &lightColor, glm::vec3 &lightPos, float &attenuationScale);

    static void Debug(const double &mouseX, const double &mouseY);

    static void DrawNode(Node* node, int& clickedMesh, const std::vector<std::unique_ptr<Mesh>>& meshes);

    static int Hierarchy(const std::vector<std::unique_ptr<Mesh>>& meshes);


};

#endif //GUI_CLASS_H
