#ifndef LSIM_ENTITYMANAGER_H
#define LSIM_ENTITYMANAGER_H

#include <vector>

#include "entity.h"

class EntityManager {
private:
        static constexpr uint32_t SENTINEL = UINT32_MAX;
        std::vector<uint32_t> generations{};
        std::vector<uint8_t> alive{};
        std::vector<uint32_t> nextFree{};
        uint32_t nextFreeHead = SENTINEL;
        uint32_t nextFreeTail = SENTINEL;

        void destroy(const Entity &e);
        friend class Registry;

        Entity create();
        [[nodiscard]] bool isAlive(const Entity e) const { return alive[e.getIndex()] && e.getGeneration() == generations[e.getIndex()]; }
};

#endif //LSIM_ENTITYMANAGER_H
