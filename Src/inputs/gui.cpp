#include "../../include/inputs/gui.h"

#include <iterator>

#include "ECS/name.h"
#include "editor/sharedState.h"
#include "geometry/transform.traits.h"
#include "gl/VAO.h"
#include "gl/VAO.h"
#include "gl/VAO.h"
#include "gl/VAO.h"
#include "include/utils/texture.h"

Gui::Node *Gui::root = nullptr;

#include <memory>
#include <algorithm>

//Map ANSI codes to there RGB values
std::unordered_map<std::string, ImColor> Gui::colourMap = {
        {Ansi::BLACK, ImColor(12, 12, 12)}, {Ansi::RED, ImColor(197, 15, 31)}, {Ansi::GREEN, ImColor(19, 161, 14)},
        {Ansi::YELLOW, ImColor(193, 156, 0)}, {Ansi::BLUE, ImColor(0, 55, 218)}, {Ansi::MAGENTA, ImColor(136, 23, 152)},
        {Ansi::CYAN, ImColor(58, 150, 221)}, {Ansi::WHITE, ImColor(204, 204, 204)},
        {Ansi::BRIGHT_RED, ImColor(255, 0, 0)}, {Ansi::BRIGHT_GREEN, ImColor(0, 255, 0)},
        {Ansi::BRIGHT_YELLOW, ImColor(255, 255, 0)}, {Ansi::BRIGHT_BLUE, ImColor(0, 0, 255)},
        {Ansi::BRIGHT_MAGENTA, ImColor(255, 0, 255)}, {Ansi::BRIGHT_CYAN, ImColor(0, 255, 255)},
        {Ansi::BRIGHT_WHITE, ImColor(255, 255, 255)}, {Ansi::INFO_COLOUR, ImColor(0, 55, 218)},
        {Ansi::WARNING_COLOUR, ImColor(193, 156, 0)}, {Ansi::ERROR_COLOUR, ImColor(197, 15, 31)}
};

void Gui::Initialize(GLFWwindow *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        const ImGuiIO &io = ImGui::GetIO();
        (void) io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        root = new Node{std::nullopt, nullptr, {}};
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

void Gui::AddTexture(const std::string &slotName, std::string &fileName,
                     Registry &registry, const std::set<EntityHandle> &currentMeshes,
                     const std::string &workingDir) {
        if (ImGui::Button(slotName.c_str())) {
                const std::string filePath = IO::OpenDialog(
                        "Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0All Files\0*.*\0");

                //Get just the fileName
                fileName = filePath.substr(filePath.find_last_of("/\\") + 1);

                //Copy the file from the file path into the project dir
                std::cout << fileName << std::endl;
                std::filesystem::copy(filePath.c_str(), std::string(workingDir + "resources/") + fileName,
                                      std::filesystem::copy_options::overwrite_existing);

                const unsigned int texture = Texture::GetTexId(
                        (std::string(workingDir + "resources/") + fileName).c_str(), GL_NEAREST);
                for (const EntityHandle &mesh: currentMeshes) {
                        auto *material = registry.getComponent<Material>(mesh);
                        if (!material) continue;
                        material->setTexture(slotName, workingDir + "resources/" + fileName);
                }
        }
}

void Gui::RemoveTexture(const std::string &slotName, Registry &registry, const std::set<EntityHandle> &currentMeshes) {
        if (ImGui::Button(slotName.c_str())) {
                for (const EntityHandle &mesh: currentMeshes) {
                        auto *material = registry.getComponent<Material>(mesh);
                        if (!material) continue;
                        material->removeTexture(slotName);
                }
        }
}

void Gui::Transform(Registry &registry, SharedState &sharedState, const std::string &workingDir, int &selectedMeshType) {
        const auto currentMeshes = sharedState.current_meshes();

        if (!currentMeshes.empty()) {
                const std::optional refMesh = *currentMeshes.begin();

                for (const auto component: registry.getComponentTypes(refMesh.value())) {
                        std::any refComponent = registry.getComponent(refMesh.value(), component);
                        registry.getInspectFunc(component)(registry, sharedState, refComponent);
                }
        }

        //         ImGui::Checkbox("Use Texture", &refMesh->useTexture);
        //
        //         //Show mesh colour if useTexture is enabled show the mesh colour otherwise show the add texture
        //         if (!refMesh->useTexture) {
        //             if (ImGui::ColorEdit4("Mesh Color", glm::value_ptr(refMesh->color))) {
        //                 for (unsigned mesh : currentMeshes) {
        //                     meshes[mesh][0].get()->useTexture = false;
        //                     meshes[mesh][0].get()->color = refMesh->color;
        //                 }
        //             }
        //         } else {
        //             static std::string fileName = "No texture";
        //
        //             //Texture
        //             AddTexture("Add Texture", fileName, meshes, currentMeshes, workingDir,
        //                 &Mesh::texId, &Mesh::texturePath, nullptr);
        //
        //             //Specular Map
        //             AddTexture("Add Specular Map", fileName, meshes, currentMeshes, workingDir,
        //                 &Mesh::specMapId, &Mesh::specMapPath, nullptr);
        //
        //             ImGui::SameLine();
        //
        //             RemoveTexture("Remove Specular Map", meshes, currentMeshes, &Mesh::specMapId, &Mesh::specMapPath, nullptr);
        //
        //             //Normal Map
        //             AddTexture("Add Normal Map", fileName, meshes, currentMeshes, workingDir,
        //                 &Mesh::normalMapId, &Mesh::normalMapPath, &Mesh::useNormalMap);
        //
        //             ImGui::SameLine();
        //
        //             RemoveTexture("Remove Normal Map", meshes, currentMeshes, &Mesh::normalMapId, &Mesh::normalMapPath, &Mesh::useNormalMap);
        //
        //             //Emissive Map
        //             AddTexture("Add Emissive Map", fileName, meshes, currentMeshes, workingDir,
        //                 &Mesh::emissiveMapId, &Mesh::emissiveMapPath, nullptr);
        //
        //             ImGui::SameLine();
        //
        //             RemoveTexture("Remove Emissive Map", meshes, currentMeshes, &Mesh::emissiveMapId, &Mesh::emissiveMapPath, nullptr);
        //         }
        //
        //         ImGui::InputFloat("Emissive Intensity", &refMesh->emissiveIntensity);
        //
        //         ImGui::SliderFloat("Roughness", &refMesh->roughness, 0, 1);
        //         ImGui::SliderFloat("F0", &refMesh->F0, 0, 1);
        //     }
        //
        //
        //
        //     static char meshSelectionBuffer[128] = "0";
        //     static int lastClickedMesh = -1;
        //
        //     if (clickedMesh != -1 && clickedMesh != lastClickedMesh) {
        //         snprintf(meshSelectionBuffer, sizeof(meshSelectionBuffer), "%d", clickedMesh);
        //         lastClickedMesh = clickedMesh;
        //     }
        //
        //     // Draw the input box
        //     ImGui::InputText("Current Meshes", meshSelectionBuffer,  IM_ARRAYSIZE(meshSelectionBuffer));
        //
        //     // Parse the buffer into currentMeshes
        //     sharedState.current_meshes().clear();
        //     std::stringstream ss(meshSelectionBuffer);
        //     std::string token;
        //     while (std::getline(ss, token, ',')) {
        //         try {
        //             int idx = std::stoi(token);
        //             if (idx >= 0 && idx < meshes.size()) {
        //                 sharedState.current_meshes().insert(idx);
        //             }
        //         } catch (...) {
        //             // ignore invalid input
        //         }
        //     }
        //
        //     const char* meshTypes[] = { "Cube", "Pyramid", "Plane", "Sphere", "Torus", "Terrain", "Model" };
        //     ImGui::Combo("Mesh Type", &selectedMeshType, meshTypes, IM_ARRAYSIZE(meshTypes));
        // }
}

void Gui::Lighting(std::vector<Light> &lights, int &currentLight) {
        if (ImGui::CollapsingHeader("Lighting")) {
                if (currentLight != -1) {
                        const char *lightTypes[] = {"Point", "Directional", "Spot"};
                        int selectedIndex = lights[currentLight].lightType;

                        if (ImGui::Combo("Light Type", &selectedIndex, lightTypes, IM_ARRAYSIZE(lightTypes))) {
                                lights[currentLight].lightType = static_cast<Light::Type>(selectedIndex);
                        }

                        ImGui::ColorEdit4("Light Color", glm::value_ptr(lights[currentLight].lightColor));

                        if (lights[currentLight].lightType != Light::directional)
                                ImGui::InputFloat3("Light Position", glm::value_ptr(lights[currentLight].lightPos));

                        if (lights[currentLight].lightType != Light::point)
                                ImGui::InputFloat3("Light Direction", glm::value_ptr(lights[currentLight].lightDir));

                        if (lights[currentLight].lightType != Light::directional)
                                ImGui::InputFloat("Light Attenuation", &lights[currentLight].attenuationScale);

                        if (lights[currentLight].lightType == Light::spotlight) {
                                ImGui::SliderAngle("Spotlight Angle", &lights[currentLight].spotAngle, -80.0f, 80.0f);
                        }

                        ImGui::InputFloat("Light Intensity", &lights[currentLight].intensity);

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

void Gui::Console(int &selectedLogLevel) {
        const char *logLevels[] = {"INFO", "WARNING", "ERROR"};

        std::vector<std::string> modules;
        for (const Logger &log: Logger::GetLogs().buffer)
                if (std::ranges::find(modules, log.GetModule()) == modules.end())
                        modules.push_back(log.GetModule());

        std::vector<const char *> modulePtrs;
        for (auto &m: modules)
                modulePtrs.push_back(m.c_str());

        ImGui::Combo("Log Level", &selectedLogLevel, logLevels, IM_ARRAYSIZE(logLevels));

        static std::vector<bool> selectedItems;
        if (selectedItems.size() != modules.size()) {
                selectedItems.assign(modules.size(), true);
        }

        if (ImGui::BeginCombo("Modules", "PREVIEW")) {
                for (int n = 0; n < static_cast<int>(modulePtrs.size()); ++n) {
                        if (ImGui::Selectable(modules[n].c_str(), selectedItems[n],
                                              ImGuiSelectableFlags_DontClosePopups)) {
                                selectedItems[n] = !selectedItems[n];
                        }
                }
                ImGui::EndCombo();
        }

        for (const Logger &log: Logger::GetLogs().buffer) {
                if (log.GetLevel() >= selectedLogLevel) {
                        auto it = std::ranges::find(modules, log.GetModule());

                        if (it != modules.end()) {
                                if (selectedItems[std::distance(modules.begin(), it)])
                                        ImGui::TextColored(colourMap[log.GetColour()], log.GetLoggerMessage().c_str());
                        }
                }
        }
}

void Gui::SceneGUI(const std::string &workingDir, unsigned int &skyboxTexId, glm::vec4 &ambientLightColour,
                   float &ambientLightIntensity) {
        if (ImGui::CollapsingHeader("Scene")) {
                if (ImGui::Button("Set Skybox")) {
                        std::array<std::string, 6> faces;
                        //Copy the skybox into resources
                        std::filesystem::copy(IO::DirectoryDialog(), workingDir + "skybox/",
                                              std::filesystem::copy_options::overwrite_existing |
                                              std::filesystem::copy_options::recursive);
                        const std::string skyBoxDir = workingDir + "skybox";

                        std::cout << skyBoxDir;

                        //Load faces
                        faces[0] = skyBoxDir + "/right.jpg";
                        faces[1] = skyBoxDir + "/left.jpg";
                        faces[2] = skyBoxDir + "/top.jpg";
                        faces[3] = skyBoxDir + "/bottom.jpg";
                        faces[4] = skyBoxDir + "/front.jpg";
                        faces[5] = skyBoxDir + "/back.jpg";

                        skyboxTexId = Texture::GetCubemapId(faces, GL_NEAREST);
                }

                ImGui::ColorEdit4("Ambient Light Colour", glm::value_ptr(ambientLightColour));
                ImGui::InputFloat("Ambient Light Intensity", &ambientLightIntensity);
        }
}

void Gui::DrawNode(Node *node, std::optional<EntityHandle> &clickedMesh, Registry &registry) {
        if (!node) return;
        std::string nodeName = "Game";
        if (node->mesh) {
                auto *name = registry.getComponent<Name>(*node->mesh);
                nodeName = (name && !name->value.empty()) ? name->value : "Unnamed";
        }
        if (ImGui::TreeNode(nodeName.c_str())) {
                if (node->mesh && ImGui::IsItemClicked()) {
                        clickedMesh = node->mesh;
                }
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                        ImGui::SetDragDropPayload("DND_NODE", &node, sizeof(Node *));
                        ImGui::Text("Moving: %s", nodeName.c_str());
                        ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_NODE")) {
                                Node *payloadNode = *static_cast<Node **>(payload->Data);
                                if (payloadNode->parent) {
                                        auto &siblings = payloadNode->parent->children;
                                        std::erase(siblings, payloadNode);
                                }
                                payloadNode->parent = node;
                                node->children.push_back(payloadNode);
                        }
                        ImGui::EndDragDropTarget();
                }
                for (Node *child: node->children) {
                        DrawNode(child, clickedMesh, registry);
                }
                ImGui::TreePop();
        }
}

void Gui::DeleteNode(Node *node) {
        if (!node || node == root) return;

        Node *parent = node->parent;
        if (!parent) return;

        // Reparent children to node's parent
        for (Node *child: node->children) {
                child->parent = parent;
                parent->children.push_back(child);
        }
        node->children.clear();

        // Remove node from parent's children
        auto &siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), node), siblings.end());

        delete node;
}

void Gui::DeleteNodeRecursively(Registry &registry, Node *node) {
        for (const auto child: node->children) {
                DeleteNodeRecursively(registry, child);
        }
        node->children = {};
        if (node != root) {
                if (!node->mesh.has_value()) { return; }
                registry.destroyEntity(node->mesh.value());
                delete node;
        }
}

void Gui::ClearRoot(Registry &registry) {
        for (auto *child: root->children) {
                DeleteNodeRecursively(registry, child);
        }
        root->children.clear();
}

Gui::Node *Gui::FindNodeByMesh(Node *node, const EntityHandle &mesh) {
        if (!node) return nullptr;
        if (node->mesh && *node->mesh == mesh) return node;
        for (Node *child: node->children) {
                if (Node *result = FindNodeByMesh(child, mesh)) return result;
        }
        return nullptr;
}

std::optional<EntityHandle> Gui::Hierarchy(Registry &registry) {
        std::optional<EntityHandle> clickedMesh;
        if (root) {
                DrawNode(root, clickedMesh, registry);
        }
        return clickedMesh;
}
