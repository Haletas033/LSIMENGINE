#include "editor/editorGUI.h"

#include "inputs/gui.h"

std::optional<LSIM::Error> EditorGUI::updateGUI(SharedState& sharedState, Scene& scene, const EntityHandle skybox, const double mouseX, const double mouseY) {
        Gui::Begin();

        const ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        const ImVec2 mainUISize(displaySize.x * 0.2f, displaySize.y);
        const ImVec2 mainUIPos(displaySize.x - mainUISize.x, 0);

        ImGui::SetNextWindowPos(mainUIPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(mainUISize, ImGuiCond_Always);

        ImGui::Begin("Main UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        Gui::Main(Registry::getDefaultRegistry(), sharedState);
        Gui::SceneGUI(skybox, scene.ambientLightColour, scene.ambientLightIntensity);

        Gui::Debug(mouseX, mouseY);

        ImGui::End();

        const ImVec2 hierarchySize(displaySize.x * 0.2f, displaySize.y);
        constexpr ImVec2 hierarchyPos(0, 0);

        ImGui::SetNextWindowPos(hierarchyPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(hierarchySize, ImGuiCond_Always);

        ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        if (std::optional<EntityHandle> clickedMesh = Gui::Hierarchy(Registry::getDefaultRegistry()); clickedMesh) sharedState.current_entities() = {clickedMesh.value()};

        ImGui::End();

        const ImVec2 consoleSize(displaySize.x * 0.6f, displaySize.y * 0.3f);
        const ImVec2 consolePos(displaySize.x * 0.2f, displaySize.y * 0.7f);

        ImGui::SetNextWindowPos(consolePos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(consoleSize, ImGuiCond_Always);

        ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        Gui::Console();

        ImGui::End();

        Gui::End();

        return std::nullopt;
}
