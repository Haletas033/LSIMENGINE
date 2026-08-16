#ifndef LSIM_MESHRENDERER_TRAITS_H
#define LSIM_MESHRENDERER_TRAITS_H

#include "meshRenderer.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<MeshRenderer> {
        static void inspect(Registry &registry, SharedState sharedState, const std::any &self) {}
        static void serialize(const std::any &transform);
        static void deserialize(const std::any &transform);
};

#endif //LSIM_MESHRENDERER_TRAITS_H
