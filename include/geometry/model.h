#ifndef MODEL_H
#define MODEL_H

#include "nlohmann/json.hpp"
#include "geometry/mesh.h"

using json = nlohmann::ordered_json;

class Model {
private:
        const char* file;
        std::vector<unsigned char> data;
        json JSON;

        std::vector<unsigned char> getData();
        std::vector<float> getFloats(json accessor);
        std::vector<uint32_t> getIndices(json accessor);

        std::expected<void, LSIM::Error> TraverseNode(Registry& registry, MeshPool& meshPool, unsigned int nextNode,
                                                      const glm::mat4& matrix);

        std::expected<void, LSIM::Error> getTextures(Registry& registry, EntityHandle& model);

        static std::vector<glm::vec2> groupFloatsVec2(const std::vector<float>& floatVec);

        static std::vector<glm::vec3> groupFloatsVec3(const std::vector<float>& floatVec);

        static std::vector<glm::vec4> groupFloatsVec4(const std::vector<float>& floatVec);

public:
        std::vector<EntityHandle> meshes;

        Model() = default;
        Model(Model&&) = default;
        Model& operator=(Model&&) = default;
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        static std::expected<Model, LSIM::Error> create(Registry& registry, MeshPool& meshPool, const char* file);
        std::expected<std::vector<EntityHandle>, LSIM::Error> loadMesh(Registry& registry, MeshPool& meshPool,
                                                                       unsigned int indMesh);
};

#endif //MODEL_H
