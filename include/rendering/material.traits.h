#ifndef LSIM_MATERIAL_TRAITS_H
#define LSIM_MATERIAL_TRAITS_H

#include "material.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<Material> {
        static void inspect(Registry &registry, SharedState sharedState, EntityHandle self);
        static void serialize(const std::any &transform);
        static void deserialize(const std::any &transform);

        template <typename T, typename Func>
        static void addPropertyField(const T &arg, Material *material, const std::string &property, Func func) {
                T v = arg;
                if (func(property.c_str(), &v)) {
                        material->setProperty(property, v);
                }
        }
};

#endif //LSIM_MATERIAL_TRAITS_H
