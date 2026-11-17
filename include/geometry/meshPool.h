#ifndef LSIM_MESHPOOL_H
#define LSIM_MESHPOOL_H
#include <cstdint>
#include <optional>
#include <vector>

#include "meshData.h"
#include "ECS/handle.h"
#include "utils/logging/log.h"
#include "vk/buffer.h"

struct MeshTag {};

using MeshHandle = Handle<MeshTag>;

enum class MeshMode {
        STATIC,
        DYNAMIC
};

struct GPUMeshBuffer {
        Buffer vertexBuffer;
        Buffer indexBuffer;
        uint32_t vertexCount;
        uint32_t indexCount;
        MeshMode mode;

        GPUMeshBuffer(
                Buffer&& vertexBuffer,
                Buffer&& indexBuffer,
                uint32_t indexCount,
                uint32_t vertexCount,
                MeshMode mode
        );
};

struct InstanceGPUData {
        glm::mat4 model;
        glm::mat3 normalMatrix;
};

struct MeshKey {
        uint64_t hash;
        uint32_t vertexCount;
        uint32_t indexCount;

        bool operator==(const MeshKey& other) const {
                return hash == other.hash &&
                        vertexCount == other.vertexCount &&
                        indexCount == other.indexCount;
        }
};

struct MeshKeyHash {
        std::size_t operator()(const MeshKey& key) const {
                std::size_t h = std::hash<uint64_t>{}(key.hash);

                h ^= std::hash<uint32_t>{}(key.vertexCount)
                        + 0x9e3779b9
                        + (h << 6)
                        + (h >> 2);

                h ^= std::hash<uint32_t>{}(key.indexCount)
                        + 0x9e3779b9
                        + (h << 6)
                        + (h >> 2);

                return h;
        }
};

class MeshPool {
private:
        static constexpr uint32_t SENTINEL = UINT32_MAX;
        std::vector<uint32_t> generations{};
        std::vector<uint8_t> alive{};
        std::vector<uint32_t> nextFree{};
        std::vector<GPUMeshBuffer> buffers;
        std::unordered_map<MeshKey, MeshHandle, MeshKeyHash> meshCache;
        std::vector<std::optional<MeshKey>> meshKeys;

        uint32_t nextFreeHead = SENTINEL;
        uint32_t nextFreeTail = SENTINEL;

        static uint64_t hashBytes(
                const void* data,
                size_t size,
                uint64_t hash = 14695981039346656037ULL
        );

        static MeshKey makeMeshKey(const MeshData& data);

        std::expected<MeshHandle, LSIM::Error> allocate(
                const Device& device,
                Command& command,
                const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, MeshMode mode
        );

public:
        [[nodiscard]] GPUMeshBuffer* get(const MeshHandle h) {
                if (!isAlive(h)) return nullptr;
                return &buffers[h.getIndex()];
        }

        [[nodiscard]] bool isAlive(const MeshHandle h) const {
                return alive[h.getIndex()] && h.getGeneration() == generations[h.getIndex()];
        }

        std::expected<MeshHandle, LSIM::Error> upload(const Device& device, Command& command, MeshData& data, MeshMode mode);
        void release(const MeshHandle& h);

        static MeshPool& getDefaultMeshPool() {
                static MeshPool instance;
                return instance;
        }
};

#endif //LSIM_MESHPOOL_H
