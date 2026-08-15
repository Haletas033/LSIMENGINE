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

#include "editor/sharedState.h"


class Gui {
private:
    static std::unordered_map<std::string, ImColor> colourMap;
public:
    struct Node {
        std::optional<EntityHandle> mesh;
        Node* parent;
        std::vector<Node*> children;
    };

    static Node* root;

    static void Initialize(GLFWwindow* window);

    static void Begin();

    static void End();

    static void CleanUp();

    static void AddTexture(const std::string &slotName, std::string &fileName,
                           Registry &registry, const std::set<EntityHandle> &currentMeshes,
                           const std::string &workingDir);
    static void RemoveTexture(const std::string &slotName,
                               Registry &registry, const std::set<EntityHandle> &currentMeshes);

    static void Transform(Registry &registry, SharedState &sharedState, const std::string &workingDir, int
                          &selectedMeshType);

    static void Lighting(std::vector<Light> &lights, int &currentLight);

    static void Debug(const double &mouseX, const double &mouseY);

    static void Console(int &selectedLogLevel);

    static void SceneGUI(const std::string &workingDir, unsigned int &skyboxTexId, glm::vec4 &ambientLightColour, float &ambientLightIntensity);

    static void DrawNode(Node *node, std::optional<EntityHandle> &clickedMesh, Registry &registry);

    static void DeleteNode(Node *node);

    static void DeleteNodeRecursively(Registry &registry, Node *node);

    static void ClearRoot(Registry &registry);

    static Node *FindNodeByMesh(Node *node, const EntityHandle &mesh);

    static Node *FindNodeByMeshID(Node *node, uint16_t meshID);

    static std::optional<EntityHandle> Hierarchy(Registry &registry);


};

#endif //GUI_CLASS_H
