#ifndef LSIM_TRANSFORM_TRAITS_H
#define LSIM_TRANSFORM_TRAITS_H

#include <any>
#include <cstdint>
#include <vector>

#include "transform.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<Transform> {
        static constexpr std::string_view id = "engine.transform";
        static void inspect(Registry &registry, SharedState &sharedState, EntityHandle self);
        static std::vector<uint8_t> serialize(Registry &registry, EntityHandle self);
        static std::any deserialize(const std::vector<uint8_t> &data, uint64_t &ptr);
};

#endif //LSIM_TRANSFORM_TRAITS_H
