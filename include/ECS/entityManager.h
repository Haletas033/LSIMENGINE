#ifndef LSIM_ENTITYMANAGER_H
#define LSIM_ENTITYMANAGER_H

#include <vector>

#include "handle.h"

struct EntityTag {};

using EntityHandle = Handle<EntityTag>;

class EntityManager {
private:
        static constexpr uint32_t SENTINEL = UINT32_MAX;
        std::vector<uint32_t> generations{};
        std::vector<uint8_t> alive{};
        std::vector<uint32_t> nextFree{};
        uint32_t nextFreeHead = SENTINEL;
        uint32_t nextFreeTail = SENTINEL;

        friend class Registry;

        EntityHandle create();
        void destroy(const EntityHandle &e);
        [[nodiscard]] bool isAlive(const EntityHandle e) const { return alive[e.getIndex()] && e.getGeneration() == generations[e.getIndex()]; }
        [[nodiscard]] std::vector<EntityHandle> getAllAlive() const;
};

#endif //LSIM_ENTITYMANAGER_H
