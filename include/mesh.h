#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>
#include <vector>

#include"VAO.h"
#include"camera.h"
#include"EBO.h"

class Mesh {
public:
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    VAO vao;

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    void SetTranslation(const glm::vec3& translation);

    void SetRotation(float angle, const glm::vec3& axis);

    void SetScale(const glm::vec3& scale);

    void ApplyTransformations(const glm::vec3& translation, float angle, const glm::vec3& axis, const glm::vec3& scale);

    Mesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);

    void Draw(Shader& shader, Camera& camera);

};
#endif //MESH_CLASS_H
