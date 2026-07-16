#include "../../include/ECS/entityManager.h"

Entity EntityManager::create() {
        uint32_t index = nextFreeHead;;
        if (nextFreeHead == SENTINEL) {
                generations.push_back(0);
                alive.push_back(true);
                nextFree.push_back(SENTINEL);
                index = generations.size()-1;
        } else {

                nextFreeHead = nextFree[index];
                if (nextFreeHead == SENTINEL) nextFreeTail = SENTINEL;
                alive[index] = true;
        }

        return Entity{index, generations[index]};
}

void EntityManager::destroy(const Entity &e) {
        if (!isAlive(e)) return;
        const auto index = e.getIndex();
        alive[index] = false;
        generations[index]++;
        if (nextFreeHead == SENTINEL && nextFreeTail == SENTINEL) {
                nextFreeHead = nextFreeTail = index;
        } else {
                nextFree[nextFreeTail] = index;
                nextFreeTail = index;
        }

        nextFree[index] = SENTINEL;
}