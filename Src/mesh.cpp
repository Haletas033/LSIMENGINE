#include "../include/mesh.h"

// Constructor
Mesh::Mesh(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices) {
    Mesh::vertices = vertices;
    Mesh::indices = indices;

    //Generate Vertex Array Object and binds it
    vao.Bind();
    //Generates Vertex Buffer Object and links it to vertices
    VBO vbo(vertices);
    //Generate Element Buffer Object and links it to indices
    EBO ebo(indices);
    //Link VBO attributes such as coordinates and colors to VAO
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //Unbind all to prevent accidentally modifying them
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Mesh::SetTranslation(const glm::vec3& translation) {
    modelMatrix = glm::translate(glm::mat4(1.0f), translation);
}

void Mesh::SetRotation(float angle, const glm::vec3& axis) {
    modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

void Mesh::SetScale(const glm::vec3& scale) {
    modelMatrix = glm::scale(glm::mat4(1.0f), scale);
}

void Mesh::ApplyTransformations(const glm::vec3& translation, float angle, const glm::vec3& axis, const glm::vec3& scale) {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, translation);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Mesh::Draw(Shader& shader, Camera& camera) {
    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    shader.Activate();
    vao.Bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}