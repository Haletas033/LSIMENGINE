#ifndef LSIM_ENTITY_H
#define LSIM_ENTITY_H
#include <cstdint>
#include <functional>

class Entity {
private:
        const uint32_t index;
        const uint32_t generation;

        Entity(const uint32_t index, const uint32_t generation) : index(index), generation(generation) {}

        friend class EntityManager;
public:
        bool operator==(const Entity &) const = default;
        
        [[nodiscard]] uint32_t getIndex() const { return index; }
        [[nodiscard]] uint32_t getGeneration() const { return generation; }
};

template<>
struct std::hash<Entity> {
        std::size_t operator()(const Entity &e) const noexcept {
                auto seed = hash<uint32_t>()(e.getIndex());
                seed ^= hash<uint32_t>()(e.getGeneration()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                return seed;
        }
};



#endif //LSIM_ENTITY_H
