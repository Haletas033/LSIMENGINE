#ifndef LSIM_MESHPOOL_H
#define LSIM_MESHPOOL_H
#include <cstdint>
#include <vector>

#include "meshData.h"
#include "ECS/handle.h"
#include "gl/EBO.h"
#include "gl/VBO.h"
#include "gl/VAO.h"
#include "utils/logging/log.h"

struct MeshTag {};

using MeshHandle = Handle<MeshTag>;

enum class MeshMode {
    STATIC,
    DYNAMIC
};

struct GPUMeshBuffer {
    VAO vao;
    VBO vbo;
    EBO ebo;
    uint32_t indexCount;
    MeshMode mode;

    GPUMeshBuffer(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const MeshMode mode) : vbo(vertices, mode == MeshMode::STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW), indexCount(indices.size()), mode(mode) {
        vao.Bind();
        vbo.Bind();
        ebo.Upload(indices);

        vao.LinkAttrib(0, 3, GL_FLOAT, 11 * sizeof(float), nullptr);
        vao.LinkAttrib(1, 3, GL_FLOAT, 11 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        vao.LinkAttrib(2, 2, GL_FLOAT, 11 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
        vao.LinkAttrib(3, 3, GL_FLOAT, 11 * sizeof(float), reinterpret_cast<void*>(8 * sizeof(float)));

        VBO::Unbind();
        VAO::Unbind();
    }
};

class MeshPool {
private:
    static constexpr uint32_t SENTINEL = UINT32_MAX;
    std::vector<uint32_t> generations{};
    std::vector<uint8_t> alive{};
    std::vector<uint32_t> nextFree{};
    std::vector<GPUMeshBuffer> buffers;

    uint32_t nextFreeHead = SENTINEL;
    uint32_t nextFreeTail = SENTINEL;

public:
    [[nodiscard]] GPUMeshBuffer* get(const MeshHandle h) {
        if (!isAlive(h)) return nullptr;
        return &buffers[h.getIndex()];
    }

    [[nodiscard]] bool isAlive(const MeshHandle h) const { return alive[h.getIndex()] && h.getGeneration() == generations[h.getIndex()]; }
    MeshHandle upload(MeshData &data, MeshMode mode);
    void release(const MeshHandle& h);

    static MeshPool& getDefaultMeshPool() {
        static MeshPool instance;
        return instance;
    }
};

#endif //LSIM_MESHPOOL_H
