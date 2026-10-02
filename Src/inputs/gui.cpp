#include "../../include/inputs/gui.h"

#include <iterator>
#include <memory>
#include <algorithm>

#include "ECS/name.h"
#include "editor/sharedState.h"
#include "geometry/transform.traits.h"

#include "include/utils/texture.h"

Gui::Node *Gui::root = nullptr;

extern std::string workingDir;

//Map ANSI codes to there RGB values
#define COL(NAME, CODE, RGB) { Ansi::NAME, ImColor RGB },

std::unordered_map<std::string, ImColor> Gui::colourMap = {
        #include "include/utils/logging/colorCodes.def"
};

#undef COL

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

void Gui::Main(Registry &registry, SharedState &sharedState) {
        const auto currentMeshes = sharedState.current_entities();

        if (!currentMeshes.empty()) {
                const std::optional refMesh = *currentMeshes.begin();

                for (const auto component: registry.getComponentTypes(refMesh.value())) {
                        registry.getInspectFunc(component)(registry, sharedState, refMesh.value());
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

void Gui::SceneGUI(EntityHandle skybox, glm::vec4 &ambientLightColour,
                   float &ambientLightIntensity) {
        if (ImGui::CollapsingHeader("Scene")) {
                if (ImGui::Button("Set Skybox")) {
                        std::array<std::string, 6> faces;
                        //Copy the skybox into resources
                        std::filesystem::copy(IO::DirectoryDialog(), workingDir + "skybox/",
                                              std::filesystem::copy_options::overwrite_existing |
                                              std::filesystem::copy_options::recursive);
                        const std::string skyBoxDir = workingDir + "skybox";

                        auto findFace = [&](const std::string& name) {
                                const auto png = std::filesystem::path(skyBoxDir) / (name + ".png");
                                const auto jpg = std::filesystem::path(skyBoxDir) / (name + ".jpg");

                                if (std::filesystem::exists(png))
                                        return png.string();

                                if (std::filesystem::exists(jpg))
                                        return jpg.string();

                                engineLogger("stdError", "Missing skybox face: " + name);
                                return std::string{};
                        };

                        // Load faces
                        faces[0] = findFace("right");
                        faces[1] = findFace("left");
                        faces[2] = findFace("top");
                        faces[3] = findFace("bottom");
                        faces[4] = findFace("front");
                        faces[5] = findFace("back");

                        Registry::getDefaultRegistry().getComponent<Material>(skybox)
                                ->setProperty("skybox", static_cast<int>(Texture::GetCubemapId(faces, GL_NEAREST)));
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

void Gui::DeleteNodeRecursively(Node* node) {
        if (!node || node == root)
                return;

        for (Node* child : node->children) {
                DeleteNodeRecursively(child);
        }

        node->children.clear();

        if (node->parent) {
                auto& siblings = node->parent->children;
                std::erase(siblings, node);
        }

        delete node;
}

void Gui::ClearRoot(Registry &registry) {
        for (auto *child: root->children) {
                DeleteNodeRecursively(child);
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
