#include <geometry/meshPool.h>

MeshHandle MeshPool::upload(MeshData &data, MeshMode mode) {
        data.GenerateTangents();

        uint32_t index = nextFreeHead;
        if (nextFreeHead == SENTINEL) {
                generations.push_back(0);
                alive.push_back(true);
                nextFree.push_back(SENTINEL);
                buffers.emplace_back(data.verticesWithTangents, data.indices, mode);
                index = generations.size() - 1;
        } else {
                nextFreeHead = nextFree[index];
                if (nextFreeHead == SENTINEL) nextFreeTail = SENTINEL;
                alive[index] = true;
                buffers[index] = GPUMeshBuffer{data.verticesWithTangents, data.indices, mode};
        }

        return MeshHandle{index, generations[index]};
}

void MeshPool::release(const MeshHandle& h) {
        if (!isAlive(h)) return;
        const auto index = h.getIndex();
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
