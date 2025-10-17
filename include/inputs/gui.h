#ifndef GUI_CLASS_H
#define GUI_CLASS_H

#include <memory>
#include<vector>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_glfw.h"
#include "../../imgui/backends/imgui_impl_opengl3.h"

#include "../geometry/mesh.h"
#include "../scene/light.h"
#include <include/utils/logging/log.h>
#include <include/utils/fileIO.h>



class Gui {
private:
    static std::unordered_map<std::string, ImColor> colourMap;
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

    static void Transform(const std::string &workingDir, const std::vector<std::vector<std::unique_ptr<Mesh>>> &meshes, std::vector<int> &currentMeshes, int &selectedMeshType, int clickedMesh);

    static void Lighting(std::vector<Light> &lights, int &currentLight);

    static void Debug(const double &mouseX, const double &mouseY);

    static void Console(int &selectedLogLevel, const std::vector<Logger> &logs);

    static void Scene(unsigned int &skyboxTexId);

    static void DrawNode(Node *node, int &clickedMesh, const std::vector<std::vector<std::unique_ptr<Mesh>>> &meshes);

    static void DeleteNode(Node *node);

    static void DeleteNodeRercursively(Node *node);

    static void ClearRoot();

    static Node *FindNodeByMesh(Node *node, const Mesh *mesh);

    static Node *FindNodeByMeshID(Node *node, uint16_t meshID);

    static int Hierarchy(const std::vector<std::vector<std::unique_ptr<Mesh>>>& meshes);


};

#endif //GUI_CLASS_H
