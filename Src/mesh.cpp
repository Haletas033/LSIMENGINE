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

void Mesh::Draw(Shader& shader, Camera& camera) {
    shader.Activate();
    vao.Bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}