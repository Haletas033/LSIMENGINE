#include <rendering/material.traits.h>

void ComponentTraits<Material>::inspect(Registry& registry, SharedState sharedState, const std::any &self) {
        if (ImGui::CollapsingHeader("Material")) {
                // TODO
        }
}