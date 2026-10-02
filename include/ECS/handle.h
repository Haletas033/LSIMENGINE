#ifndef LSIM_HANDLE_H
#define LSIM_HANDLE_H
#include <cstdint>

template<typename Tag>
class Handle {
private:
    uint32_t index{};
    uint32_t generation{};

    Handle(const uint32_t index, const uint32_t generation) : index(index), generation(generation){}

    friend class EntityManager;
    friend class MeshPool;

public:
    bool operator==(const Handle &) const = default;

    bool operator<(const Handle& other) const {
        if (index != other.index) return index < other.index;
        return generation < other.generation;
    }

    [[nodiscard]] uint32_t getIndex() const { return index; }
    [[nodiscard]] uint32_t getGeneration() const { return generation; }
};

template<typename Tag>
struct std::hash<Handle<Tag>> {
    std::size_t operator()(const Handle<Tag>& h) const noexcept {
        auto seed = std::hash<uint32_t>()(h.getIndex());
        seed ^= std::hash<uint32_t>()(h.getGeneration()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

#endif //LSIM_HANDLE_H
