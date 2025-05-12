#include "../include/VBO.h"

//Constructor that generates a vertex buffer object and links it to vertices
VBO::VBO(std::vector<GLfloat>& vertices) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
}
//Binds the VBO
void VBO::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}
//Unbinds the VBO
void VBO::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
//Deletes the VBO
void VBO::Delete() {
    glDeleteBuffers(1, &ID);
}

