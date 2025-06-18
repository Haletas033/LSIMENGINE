#include"../include/gui.h"

void Gui::Initialize(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
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

void Gui::Transform(std::vector<Mesh> &meshes, int &currentMesh, int &selectedMeshType) {
    if (ImGui::CollapsingHeader("Transform")){
        if (!meshes.empty()) {
            ImGui::InputFloat3("Position", glm::value_ptr(meshes[currentMesh].position));
            ImGui::InputFloat3("Rotation", glm::value_ptr(meshes[currentMesh].rotation));

            static bool uniformScaleLock = true;

            static float uniformScale = meshes[currentMesh].scale.x;  // Initial uniform scale

            if (uniformScaleLock) {
                if (ImGui::InputFloat("Scale", &uniformScale, 0.1f)) {
                    meshes[currentMesh].scale = glm::vec3(uniformScale);
                }
            } else {
                ImGui::InputFloat3("Scale", glm::value_ptr(meshes[currentMesh].scale));
            }

            ImGui::Checkbox("Uniform Scale", &uniformScaleLock);

            if (!meshes[currentMesh].useTexture) {
                ImGui::ColorEdit4("Mesh Color", glm::value_ptr(meshes[currentMesh].color));
            }

            meshes[currentMesh].ApplyTransformations(); // Apply on button click

            ImGui::InputInt("Current Mesh", &currentMesh);
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

