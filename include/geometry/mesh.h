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
    glm::vec4 color = glm::vec4(1.0f);
    GLuint texId{};
    uint16_t meshID = 0;

    VAO vao;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale    = glm::vec3(1.0f);

    glm::mat4 modelMatrix = glm::mat4(1.0f);


    void ApplyTransformations();

    Mesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);
    Mesh() = default;

    void Draw(Shader& shader, Camera& camera);

    void setupBuffers();


};
#endif //MESH_CLASS_H
