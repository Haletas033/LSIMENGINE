#include <ECS/name.traits.h>

void ComponentTraits<Name>::inspect(Registry& registry, SharedState sharedState, const std::any &self) {
        const auto &[name] = std::any_cast<Name>(self);

        static char nameBuffer[128];
        strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer)-1] = '\0';

        if (ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
                for (const auto e : sharedState.current_meshes()) {
                        if (!registry.hasComponent<Name>(e)) { continue; }
                        registry.getComponent<Name>(e)->value = nameBuffer;
                }
        }
}