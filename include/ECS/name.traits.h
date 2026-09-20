#ifndef LSIM_NAME_TRAITS_H
#define LSIM_NAME_TRAITS_H

#include <cstring>

#include "name.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"


template <>
struct ComponentTraits<Name> {
        static constexpr std::string_view id = "engine.name";
        static void inspect(Registry &registry, SharedState &sharedState, EntityHandle self);
        static std::vector<uint8_t> serialize(Registry &registry, EntityHandle self);
        static std::any deserialize(const std::vector<uint8_t> &data, uint64_t &ptr);
};

#endif //LSIM_NAME_TRAITS_H
