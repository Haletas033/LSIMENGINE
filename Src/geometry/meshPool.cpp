#include <geometry/meshPool.h>

void GPUMeshBuffer::configureInstanceAttributes(const GLuint instanceVBO) const {
        vao.Bind();

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

        // mat4 model -> locations 4, 5, 6, 7
        for (GLuint i = 0; i < 4; ++i) {
                glEnableVertexAttribArray(4 + i);

                glVertexAttribPointer(
                    4 + i,
                    4,
                    GL_FLOAT,
                    GL_FALSE,
                    sizeof(InstanceGPUData),
                    reinterpret_cast<void*>(
                        offsetof(InstanceGPUData, model) +
                        sizeof(glm::vec4) * i
                    )
                );

                glVertexAttribDivisor(4 + i, 1);
        }

        // mat3 normalMatrix -> locations 8, 9, 10
        for (GLuint i = 0; i < 3; ++i) {
                glEnableVertexAttribArray(8 + i);

                glVertexAttribPointer(
                    8 + i,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    sizeof(InstanceGPUData),
                    reinterpret_cast<void*>(
                        offsetof(InstanceGPUData, normalMatrix) +
                        sizeof(glm::vec3) * i
                    )
                );

                glVertexAttribDivisor(8 + i, 1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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
                data.verticesWithTangents.data(),
                data.verticesWithTangents.size() * sizeof(float),
                hash
        );

        hash = hashBytes(
                data.indices.data(),
                data.indices.size() * sizeof(uint32_t),
                hash
        );

        return MeshKey{
                hash,
                static_cast<uint32_t>(data.verticesWithTangents.size()),
                static_cast<uint32_t>(data.indices.size())
        };
}

MeshHandle MeshPool::allocate(const std::vector<float> &vertices, const std::vector<uint32_t> &indices, MeshMode mode) {
        uint32_t index = nextFreeHead;
        if (nextFreeHead == SENTINEL) {
                generations.push_back(0);
                alive.push_back(true);
                nextFree.push_back(SENTINEL);
                meshKeys.emplace_back(std::nullopt);
                buffers.emplace_back(vertices, indices, mode);
                index = generations.size() - 1;
        } else {
                nextFreeHead = nextFree[index];
                if (nextFreeHead == SENTINEL) nextFreeTail = SENTINEL;
                alive[index] = true;
                buffers[index] = GPUMeshBuffer{vertices, indices, mode};
        }

        return MeshHandle{index, generations[index]};
}

MeshHandle MeshPool::upload(MeshData &data, const MeshMode mode) {
        data.GenerateTangents();

        if (mode == MeshMode::STATIC) {
                const MeshKey key = makeMeshKey(data);

                if (const auto it = meshCache.find(key); it != meshCache.end())
                        return it->second;

                const MeshHandle handle = allocate(data.verticesWithTangents, data.indices, mode);

                meshCache.emplace(key, handle);

                meshKeys[handle.getIndex()] = key;

                return handle;
        }

        return allocate(
                data.verticesWithTangents,
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
