#include <ECS/entityManager.h>

EntityHandle EntityManager::create() {
        uint32_t index = nextFreeHead;
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

        return EntityHandle{index, generations[index]};
}

void EntityManager::destroy(const EntityHandle &e) {
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

std::vector<EntityHandle> EntityManager::getAllAlive() const {
        std::vector<EntityHandle> result;
        for (uint32_t i = 0; i < generations.size(); ++i) {
                if (alive[i]) result.push_back(EntityHandle{i, generations[i]});
        }
        return result;
}