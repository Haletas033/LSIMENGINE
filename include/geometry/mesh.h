#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>
#include <vector>

#include "../gl/VAO.h"
#include "../scene/camera.h"
#include "../gl/EBO.h"


class Mesh {
public:
    std::string name;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    bool useTexture = false;
    bool useNormalMap = false;

    glm::vec4 color = glm::vec4(1.0f);

    float roughness = 0.5f;
    float F0 = 0.03;

    //Textures

    GLuint texId{};
    std::string texturePath;

    GLuint specMapId{};
    std::string specMapPath;

    GLuint normalMapId{};
    std::string normalMapPath;

    GLuint emissiveMapId{};
    std::string emissiveMapPath;

    float emissiveIntensity = 1.f;

    uint16_t meshID = 0;

    VAO vao;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale    = glm::vec3(1.0f);

    glm::mat4 modelMatrix = glm::mat4(1.0f);


    void ApplyTransformations();

    Mesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);
    Mesh() = default;

    void Draw(::Shader &shader, ::Camera &camera, const glm::mat4 &finalMatrix);

    //Helper to generate the vecs needed for tangents
    template <glm::length_t N>
    using vec = glm::vec<N, float>;
    template <glm::length_t N>
    vec<N> VecFromVertices(const int STRIDE, const GLuint iN, const int offset = 0) {
        vec<N> p;

        for (int i = 0; i < N; i++) {
            p[i] = vertices[iN*STRIDE+i+offset];
        }

        return p;
    }

    void GenerateTangents();

    void setupBuffers();


};
#endif //MESH_CLASS_H
