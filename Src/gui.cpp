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

            Mesh& refMesh = *meshes[currentMeshes[0]];

            static glm::vec3 lastPosition = refMesh.position;
            static glm::vec3 lastRotation = refMesh.rotation;
            static glm::vec3 lastScale = refMesh.scale;

            glm::vec3 newPosition = lastPosition;
            glm::vec3 newRotation = lastRotation;
            glm::vec3 newScale = lastScale;

            static bool uniformScaleLock = true;
            static float uniformScale = refMesh.scale.x;  // Initial uniform scale

            static char nameBuffer[128];

            if (!currentMeshes.empty()) {
                strncpy(nameBuffer, refMesh.name.c_str(), sizeof(nameBuffer));
                nameBuffer[sizeof(nameBuffer)-1] = '\0';
            }

            if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
                for (const int mesh : currentMeshes) meshes[mesh].get()->name = nameBuffer;
            }

            if (ImGui::InputFloat3("Position", glm::value_ptr(newPosition))) {
                const glm::vec3 delta = newPosition - lastPosition;
                for (const int mesh : currentMeshes) {
                    meshes[mesh].get()->position += delta;
                }
                lastPosition = newPosition;
            }
            if (ImGui::InputFloat3("Rotation", glm::value_ptr(newRotation))) {
                const glm::vec3 delta = newRotation - lastRotation;
                for (const int mesh : currentMeshes) {
                    meshes[mesh].get()->rotation += delta;
                }
                lastRotation = newRotation;
            }

            if (uniformScaleLock) {
                if (ImGui::InputFloat("Scale", &uniformScale, 0.1f)) {
                    for (const int mesh : currentMeshes) {
                        meshes[mesh].get()->scale = glm::vec3(uniformScale);
                    }
                    lastScale = glm::vec3(uniformScale);
                }
            } else {
                if (ImGui::InputFloat3("Scale", glm::value_ptr(newScale))) {
                    const glm::vec3 delta = newScale - lastScale;
                    for (const int mesh : currentMeshes) {
                        meshes[mesh].get()->scale += delta;
                    }
                    lastScale = newScale;
                }
            }

            ImGui::Checkbox("Uniform Scale", &uniformScaleLock);


            if (!refMesh.useTexture) {
                if (ImGui::ColorEdit4("Mesh Color", glm::value_ptr(refMesh.color))) {
                    for (int mesh : currentMeshes) {
                        meshes[mesh].get()->color = refMesh.color;
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

void Gui::Lighting(glm::vec4 &lightColor, glm::vec3 &lightPos, float &attenuationScale) {
    if (ImGui::CollapsingHeader("Lighting")) {
        ImGui::ColorEdit4("Light Color", glm::value_ptr(lightColor));

        ImGui::InputFloat3("Light Position", glm::value_ptr(lightPos));

        ImGui::InputFloat("Light Attenuation", &attenuationScale);
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

int Gui::Hierarchy(const std::vector<std::unique_ptr<Mesh>>& meshes) {
    int clickedMesh = -1;
    if (root) {
        DrawNode(root, clickedMesh, meshes);
    }
    return clickedMesh;
}


