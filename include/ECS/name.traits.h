#ifndef LSIM_NAME_TRAITS_H
#define LSIM_NAME_TRAITS_H

#include "name.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<Name> {
        static void inspect(Registry &registry, SharedState sharedState, EntityHandle self);
        static void serialize(const std::any &transform);
        static void deserialize(const std::any &transform);
};

#endif //LSIM_NAME_TRAITS_H
