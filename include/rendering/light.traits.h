#ifndef LSIM_LIGHT_TRAITS_H
#define LSIM_LIGHT_TRAITS_H

#ifndef LSIM_MESHRENDERER_TRAITS_H
#define LSIM_MESHRENDERER_TRAITS_H

#include "light.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<Light> {
        static constexpr std::string_view id = "engine.light";
        static void inspect(Registry &registry, SharedState &sharedState, const EntityHandle &self);
        static std::vector<uint8_t> serialize(Registry &registry, EntityHandle self);
        static std::any deserialize(const std::vector<uint8_t> &data, uint64_t &ptr);
};

#endif //LSIM_MESHRENDERER_TRAITS_H


#endif //LSIM_LIGHT_TRAITS_H
