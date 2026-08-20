#include <rendering/material.traits.h>

void ComponentTraits<Material>::inspect(Registry& registry, SharedState sharedState, const EntityHandle self) {
        if (ImGui::CollapsingHeader("Material")) {
                auto material = registry.getComponent<Material>(self);
                // TODO
        }
}