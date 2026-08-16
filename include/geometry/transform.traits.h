#ifndef LSIM_TRANSFORM_TRAITS_H
#define LSIM_TRANSFORM_TRAITS_H
#include "transform.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<Transform> {
        static void inspect(Registry &registry, SharedState sharedState, const std::any &self);
        static void serialize(const std::any &transform);
        static void deserialize(const std::any &transform);
};

#endif //LSIM_TRANSFORM_TRAITS_H
