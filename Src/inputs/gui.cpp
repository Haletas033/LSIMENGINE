#include "../../include/inputs/gui.h"

#include <iterator>

Gui::Node* Gui::root = nullptr;

#include <memory>
#include <algorithm>

//Map ANSI codes to there RGB values
std::unordered_map<std::string, ImColor> Gui::colourMap = {
    {BLACK, ImColor(12, 12, 12)}, {RED, ImColor(197, 15, 31)}, {GREEN, ImColor(19, 161, 14)},
    {YELLOW, ImColor(193, 156, 0)}, {BLUE, ImColor(0, 55, 218)}, {MAGENTA, ImColor(136, 23, 152)},
    {CYAN, ImColor(58, 150, 221)}, {WHITE, ImColor(204, 204, 204)},
    {BRIGHT_RED, ImColor(255, 0, 0)}, {BRIGHT_GREEN, ImColor(0, 255, 0)},
    {BRIGHT_YELLOW, ImColor(255, 255, 0)}, {BRIGHT_BLUE, ImColor(0, 0, 255)},
    {BRIGHT_MAGENTA, ImColor(255, 0, 255)}, {BRIGHT_CYAN, ImColor(0, 255, 255)},
    {BRIGHT_WHITE, ImColor(255, 255, 255)}, {INFO_COLOUR, ImColor(0, 55, 218)},
    {WARNING_COLOUR, ImColor(193, 156, 0)}, {ERROR_COLOUR, ImColor(197, 15, 31)}
};

void Gui::Initialize(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    root = new Node{nullptr, nullptr, {}};
}


void Gui::Begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::End() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::CleanUp() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Transform(const std::vector<std::unique_ptr<Mesh>>& meshes, std::vector<int> &currentMeshes, int &selectedMeshType, int clickedMesh) {
    if (ImGui::CollapsingHeader("Transform")){
        if (!meshes.empty()) {

            Mesh* refMesh = nullptr;
            if (!currentMeshes.empty()){
                refMesh = meshes[currentMeshes[0]].get();
            }

            if (refMesh){
                glm::vec3 position = refMesh->position;
                glm::vec3 rotation = refMesh->rotation;
                glm::vec3 scale = refMesh->scale;
    
                static bool uniformScaleLock = true;
                static float uniformScale = refMesh->scale.x;  // Initial uniform scale
    
                static char nameBuffer[128];
    
                if (!currentMeshes.empty()) {
                    strncpy(nameBuffer, refMesh->name.c_str(), sizeof(nameBuffer));
                    nameBuffer[sizeof(nameBuffer)-1] = '\0';
                }
    
                if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
                    for (const int mesh : currentMeshes) meshes[mesh].get()->name = nameBuffer;
                }
    
                if (ImGui::InputFloat3("Position", glm::value_ptr(position))) {
                    for (int idx : currentMeshes) {
                        meshes[idx]->position = position;
                    }
                }
                if (ImGui::InputFloat3("Rotation", glm::value_ptr(rotation))) {
                    for (int idx : currentMeshes) {
                        meshes[idx]->rotation = rotation;
                    }
                }
    
                if (uniformScaleLock) {
                    if (ImGui::InputFloat("Scale", &uniformScale, 0.1f)) {
                        for (int idx : currentMeshes) {
                            meshes[idx]->scale = glm::vec3(uniformScale);
                        }
                    }
                } else {
                    if (ImGui::InputFloat3("Scale", glm::value_ptr(scale))) {
                        for (int idx : currentMeshes) {
                            meshes[idx]->scale = scale;
                        }
                    }
                }
    
                ImGui::Checkbox("Uniform Scale", &uniformScaleLock);
    
    
                if (!refMesh->useTexture) {
                    if (ImGui::ColorEdit4("Mesh Color", glm::value_ptr(refMesh->color))) {
                        for (int mesh : currentMeshes) {
                            meshes[mesh].get()->color = refMesh->color;
                        }
                    }
                }
            }

            for (const int mesh : currentMeshes) meshes[mesh].get()->ApplyTransformations(); // Apply on button click

            static char meshSelectionBuffer[128] = "";
            static int lastClickedMesh = -1;

            if (clickedMesh != -1 && clickedMesh != lastClickedMesh) {
                snprintf(meshSelectionBuffer, sizeof(meshSelectionBuffer), "%d", clickedMesh);
                lastClickedMesh = clickedMesh;
            }

            // Draw the input box
            ImGui::InputText("Current Meshes", meshSelectionBuffer,  IM_ARRAYSIZE(meshSelectionBuffer));

            // Parse the buffer into currentMeshes
            currentMeshes.clear();
            std::stringstream ss(meshSelectionBuffer);
            std::string token;
            while (std::getline(ss, token, ',')) {
                try {
                    int idx = std::stoi(token);
                    if (idx >= 0 && idx < meshes.size()) {
                        currentMeshes.push_back(idx);
                    }
                } catch (...) {
                    // ignore invalid input
                }
            }
        }

        const char* meshTypes[] = { "Cube", "Pyramid", "Plane", "Sphere", "Torus", "Terrain" };
        ImGui::Combo("Mesh Type", &selectedMeshType, meshTypes, IM_ARRAYSIZE(meshTypes));
    }
}

void Gui::Lighting(std::vector<Light> &lights, int &currentLight) {
    if (ImGui::CollapsingHeader("Lighting")) {
        if (currentLight != -1) {
            ImGui::ColorEdit4("Light Color", glm::value_ptr(lights[currentLight].lightColor));

            ImGui::InputFloat3("Light Position", glm::value_ptr(lights[currentLight].lightPos));

            ImGui::InputFloat("Light Attenuation", &lights[currentLight].attenuationScale);
            lights[currentLight].invScale  = 1.0f   / (lights[currentLight].attenuationScale + 0.001f);
            lights[currentLight].linear    = 0.09f  * lights[currentLight].invScale;
            lights[currentLight].quadratic = 0.032f * lights[currentLight].invScale;

            if (ImGui::InputInt("Current Light", &currentLight))
                currentLight = std::clamp(currentLight, 0, static_cast<int>(lights.size() - 1));
        } else {
            ImGui::Text("No lights exist in the current scene.");
        }
    }
}

void Gui::Debug(const double &mouseX, const double &mouseY) {
    if (ImGui::CollapsingHeader("Mouse and FPS")) {
        ImGui::Text("Mouse X: %.2f", mouseX);
        ImGui::Text("Mouse Y: %.2f", mouseY);
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    }
}

void Gui::Console(int &selectedLogLevel, const std::vector<Logger> &logs) {
    const char* logLevels[] = { "INFO", "WARNING", "ERROR" };

    std::vector<std::string> modules;
    for (Logger log : logs)
        if (std::find(modules.begin(), modules.end(), log.GetModule()) == modules.end())
            modules.push_back(log.GetModule());

    std::vector<const char*> modulePtrs;
    for (auto& m : modules)
        modulePtrs.push_back(m.c_str());

    ImGui::Combo("Log Level", &selectedLogLevel, logLevels, IM_ARRAYSIZE(logLevels));

    static std::vector<bool> selectedItems;
    if (selectedItems.size() != modules.size()) {
        selectedItems.assign(modules.size(), true);
    }

    if (ImGui::BeginCombo("Modules", "PREVIEW")) {
        for (int n = 0; n < static_cast<int>(modulePtrs.size()); ++n) {
            if (ImGui::Selectable(modules[n].c_str(), selectedItems[n], ImGuiSelectableFlags_DontClosePopups)) {
                selectedItems[n] = !selectedItems[n];
            }
        }
        ImGui::EndCombo();
    }



    for (Logger log : logs) {
        if (log.GetLevel() >= selectedLogLevel) {
            auto it = std::find(modules.begin(), modules.end(), log.GetModule());

            if (it != modules.end()) {
                if (selectedItems[std::distance(modules.begin(), it)])
                    ImGui::TextColored(colourMap[log.GetColour()], log().c_str());
            }
        }
    }
}


void Gui::DrawNode(Node* node, int& clickedMesh, const std::vector<std::unique_ptr<Mesh>>& meshes) {
    if (!node) return;

    auto nodeName = "Game";
    if (node->mesh) nodeName = node->mesh->name.empty() ? "Unnamed" : node->mesh->name.c_str();

    // Create cool hierarchy tree
    if (ImGui::TreeNode(nodeName)) {

        // Deal with clicking
        if (node->mesh) {
            int index = -1;
            for (int i = 0; i < meshes.size(); i++) {
                if (meshes[i].get() == node->mesh) {
                    index = i;
                    break;
                }
            }
            if (index != -1 && ImGui::IsItemClicked()) clickedMesh = index;
        }

        // First we drag then we drop
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("DND_NODE", &node, sizeof(Node*));
            ImGui::Text("Moving: %s", nodeName);
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_NODE")) {
                Node* payloadNode = *static_cast<Node **>(payload->Data);

                if (payloadNode->parent) {
                    auto& siblings = payloadNode->parent->children;
                    siblings.erase(std::remove(siblings.begin(), siblings.end(), payloadNode), siblings.end());
                }

                payloadNode->parent = node;
                node->children.push_back(payloadNode);
            }
            ImGui::EndDragDropTarget();
        }

        // Draw children recursively
        for (Node* child : node->children) {
            DrawNode(child, clickedMesh, meshes);
        }

        ImGui::TreePop();
    }
}

void Gui::DeleteNode(Node* node) {
    if (!node || node == root) return;

    Node* parent = node->parent;
    if (!parent) return;

    // Reparent children to node's parent
    for (Node* child : node->children) {
        child->parent = parent;
        parent->children.push_back(child);
    }
    node->children.clear();

    // Remove node from parent's children
    auto& siblings = parent->children;
    siblings.erase(std::remove(siblings.begin(), siblings.end(), node), siblings.end());

    delete node;
}

void Gui::DeleteNodeRercursively(Node* node) {
    for (const auto child : node->children) {
        DeleteNodeRercursively(child);
    }
    node->children = {};
    if (node != root)
        delete node;
}

void Gui::ClearRoot() {
    for (auto* child : root->children) {
        DeleteNodeRercursively(child);
    }
    root->children.clear();
}


Gui::Node* Gui::FindNodeByMesh(Node* node, const Mesh* mesh) {
    if (!node) return nullptr;
    if (node->mesh == mesh) return node;
    for (Node* child : node->children) {
        Node* result = FindNodeByMesh(child, mesh);
        if (result) return result;
    }
    return nullptr;
}

Gui::Node *Gui::FindNodeByMeshID(Node *node, const uint16_t meshID) {
    if (!node) return nullptr;
    if (node->mesh && node->mesh->meshID == meshID) return node;
    for (Node* child : node->children) {
        Node* result = FindNodeByMeshID(child, meshID);
        if (result) return result;
    }
    return nullptr;
}


int Gui::Hierarchy(const std::vector<std::unique_ptr<Mesh>>& meshes) {
    int clickedMesh = -1;
    if (root) {
        DrawNode(root, clickedMesh, meshes);
    }
    return clickedMesh;
}


