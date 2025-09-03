#include"../include/gui.h"

Gui::Node* Gui::root = nullptr;

#include <memory>
#include <algorithm>

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
        ImGui::ColorEdit4("Light Color", glm::value_ptr(lights[currentLight].lightColor));

        ImGui::InputFloat3("Light Position", glm::value_ptr(lights[currentLight].lightPos));

        ImGui::InputFloat("Light Attenuation", &lights[currentLight].attenuationScale);

        ImGui::InputInt("Current Light", &currentLight);
    }
}

void Gui::Debug(const double &mouseX, const double &mouseY) {
    if (ImGui::CollapsingHeader("Mouse and FPS")) {
        ImGui::Text("Mouse X: %.2f", mouseX);
        ImGui::Text("Mouse Y: %.2f", mouseY);
        ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
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

Gui::Node* Gui::FindNodeByMesh(Node* node, const Mesh* mesh) {
    if (!node) return nullptr;
    if (node->mesh == mesh) return node;
    for (Node* child : node->children) {
        Node* result = FindNodeByMesh(child, mesh);
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


