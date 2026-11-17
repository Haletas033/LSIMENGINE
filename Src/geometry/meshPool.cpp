#include <geometry/meshPool.h>

#include "vk/command.h"

uint64_t MeshPool::hashBytes(const void *data, const size_t size, uint64_t hash) {
        const auto* bytes = static_cast<const uint8_t*>(data);

        for (size_t i = 0; i < size; ++i) {
                hash ^= bytes[i];
                hash *= 1099511628211ULL;
        }

        return hash;

}

MeshKey MeshPool::makeMeshKey(const MeshData &data) {
        uint64_t hash = 14695981039346656037ULL;

        hash = hashBytes(
                data.vertices.data(),
                data.vertices.size() * sizeof(Vertex),
                hash
        );

        hash = hashBytes(
                data.indices.data(),
                data.indices.size() * sizeof(uint32_t),
                hash
        );

        return MeshKey{
                hash,
                static_cast<uint32_t>(data.vertices.size()),
                static_cast<uint32_t>(data.indices.size())
        };
}

std::expected<MeshHandle, LSIM::Error> MeshPool::allocate(
        const Device& device,
        Command& command,
        const std::vector<Vertex> &vertices,
        const std::vector<uint32_t> &indices,
        MeshMode mode
) {
        const VkDeviceSize vertexSize = vertices.size() * sizeof(Vertex);
        const VkDeviceSize indexSize = indices.size() * sizeof(uint32_t);

        auto vertexBuffer = Buffer::createFromData(
                device,
                command,
                vertices.data(),
                vertexSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        );

        if (!vertexBuffer)
                return std::unexpected(vertexBuffer.error());

        auto indexBuffer = Buffer::createFromData(
                device,
                command,
                indices.data(),
                indexSize,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        );

        if (!indexBuffer)
                return std::unexpected(indexBuffer.error());

        uint32_t index = nextFreeHead;

        GPUMeshBuffer gpuBuffer{
                std::move(*vertexBuffer),
                std::move(*indexBuffer),
                static_cast<uint32_t>(indices.size()),
                static_cast<uint32_t>(vertices.size()),
                mode
        };

        if (nextFreeHead == SENTINEL) {
                generations.push_back(0);
                alive.push_back(true);
                nextFree.push_back(SENTINEL);
                meshKeys.emplace_back(std::nullopt);
                buffers.push_back(std::move(gpuBuffer));
                index = generations.size() - 1;
        } else {
                nextFreeHead = nextFree[index];
                if (nextFreeHead == SENTINEL) nextFreeTail = SENTINEL;
                alive[index] = true;
                buffers[index] = std::move(gpuBuffer);
        }

        return MeshHandle{index, generations[index]};
}

std::expected<MeshHandle, LSIM::Error> MeshPool::upload(
        const Device& device,
        Command& command,
        MeshData& data,
        const MeshMode mode
) {
        data.GenerateTangents();

        if (mode == MeshMode::STATIC) {
                const MeshKey key = makeMeshKey(data);

                if (const auto it = meshCache.find(key); it != meshCache.end())
                        return it->second;

                const auto handle = allocate(device, command, data.vertices, data.indices, mode);

                if (!handle)
                        return std::unexpected(handle.error());

                meshCache.emplace(key, *handle);

                meshKeys[handle->getIndex()] = key;

                return handle;
        }

        return allocate(
                device,
                command,
                data.vertices,
                data.indices,
                mode
        );
}

void MeshPool::release(const MeshHandle& h) {
        if (!isAlive(h)) return;
        const auto index = h.getIndex();

        if (meshKeys[index].has_value()) {
                meshCache.erase(*meshKeys[index]);
                meshKeys[index].reset();
        }

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
