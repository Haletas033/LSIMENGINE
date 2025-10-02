//
// Created by halet on 10/2/2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <external/nlohmann/json.hpp>
#include <include/geometry/mesh.h>

using json = nlohmann::json;

class Model {
private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;

    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);

    void TraverseNode(const unsigned int nextNode, const glm::mat4 &matrix);

    void getTextures(Mesh &model);

    static std::vector<glm::vec2> groupFloatsVec2(const std::vector<float> &floatVec);

    static std::vector<glm::vec3> groupFloatsVec3(const std::vector<float> &floatVec);

    static std::vector<glm::vec4> groupFloatsVec4(const std::vector<float> &floatVec);


public:
    std::vector<Mesh> meshes;

    explicit Model(const char* file);

    Mesh loadMesh(unsigned int indMesh);


};

#endif //MODEL_H
