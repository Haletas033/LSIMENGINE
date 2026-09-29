#include "rendering/light.traits.h"

std::any ComponentTraits<Light>::deserialize(const std::vector<uint8_t> &data, uint64_t &ptr) {

}

std::vector<uint8_t> ComponentTraits<Light>::serialize(Registry &registry, EntityHandle self) {

}
