#include <cstring>
#include <rendering/meshRenderer.traits.h>



std::vector<uint8_t> ComponentTraits<MeshRenderer>::serialize(Registry& registry, const EntityHandle self) {
        const auto renderer = registry.getComponent<MeshRenderer>(self);
        std::vector<uint8_t> data{};

        const auto append = [&data](const void* ptr, const size_t size) {
                const auto* bytes = static_cast<const uint8_t*>(ptr);
                data.insert(data.end(), bytes, bytes + size);
        };

        const auto vertexCount = static_cast<std::uint32_t>(renderer->vertices.size());

        append(&vertexCount, sizeof(vertexCount));

        if (vertexCount > 0) {
                append(
                    renderer->vertices.data(),
                    renderer->vertices.size() * sizeof(float)
                );
        }

        const auto indexCount = static_cast<std::uint32_t>(renderer->indices.size());

        append(&indexCount, sizeof(indexCount));

        if (indexCount > 0) {
                append(
                    renderer->indices.data(),
                    renderer->indices.size() * sizeof(std::uint32_t)
                );
        }

        const auto mode = renderer->mode;
        append(&mode, sizeof(mode));

        return data;
}

std::any ComponentTraits<MeshRenderer>::deserialize(const std::vector<uint8_t> &data, size_t &ptr) {
        const auto read = [&data, &ptr](void* dst, const size_t size) {
                if (ptr > data.size() || size > data.size() - ptr) {
                        throw std::runtime_error("Invalid MeshRenderer data");
                }

                std::memcpy(dst, data.data() + ptr, size);
                ptr += size;
        };

        std::uint32_t vertexCount;
        read(&vertexCount, sizeof(vertexCount));

        std::vector<float> vertices;
        vertices.resize(vertexCount);

        if (vertexCount > 0) {
                read(
                    vertices.data(),
                    vertexCount * sizeof(float)
                );
        }

        std::uint32_t indexCount;
        read(&indexCount, sizeof(indexCount));

        std::vector<std::uint32_t> indices;
        indices.resize(indexCount);

        if (indexCount > 0) {
                read(
                    indices.data(),
                    indexCount * sizeof(std::uint32_t)
                );
        }

        MeshMode mode;
        read(&mode, sizeof(mode));

        MeshData meshData{
                vertices,
                {},
                indices
            };

        const MeshHandle meshHandle = MeshPool::getDefaultMeshPool().upload(
            meshData,
            mode
        );

        return MeshRenderer{
                .meshHandle = meshHandle,
                .vertices = std::move(vertices),
                .indices = std::move(indices),
                .mode = mode
        };
}