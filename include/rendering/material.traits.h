#ifndef LSIM_MATERIAL_TRAITS_H
#define LSIM_MATERIAL_TRAITS_H

#include "material.h"
#include "ECS/componentTraits.h"
#include "editor/sharedState.h"

template <>
struct ComponentTraits<Material> {
        static constexpr std::string_view id = "engine.material";
        static void inspect(Registry &registry, SharedState &sharedState, EntityHandle self);

        template <typename T, typename Func>
        static void addPropertyField(const T &arg, Material *material, const std::string &property, Func func) {
                T v = arg;
                if (func(property.c_str(), &v)) {
                        material->setProperty(property, v);
                }
        }

        static std::vector<uint8_t> serialize(Registry &registry, EntityHandle self);
        static std::any deserialize(const std::vector<uint8_t> &data, uint64_t &ptr);
};

#endif //LSIM_MATERIAL_TRAITS_H
