#ifndef LSIM_MESHRENDERER_TRAITS_H
#define LSIM_MESHRENDERER_TRAITS_H

#include "meshRenderer.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<MeshRenderer> {
        static constexpr std::string_view id = "engine.mesh_renderer";
        static void inspect(Registry &registry, SharedState &sharedState, const std::any &self) {}
        static std::vector<uint8_t> serialize(Registry &registry, EntityHandle self);
        static std::any deserialize(const std::vector<uint8_t> &data, uint64_t &ptr);
};

#endif //LSIM_MESHRENDERER_TRAITS_H
