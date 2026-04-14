#include "../../include/gl/VAO.h"

VAO::VAO() {
    glGenVertexArrays(1, &ID);
}

VAO::VAO(VAO &&other) noexcept {
    this->ID = other.ID;
    other.ID = 0;
}

VAO &VAO::operator=(VAO &&other) noexcept {
    if (this == &other) return *this;
    Delete();
    ID = other.ID;
    other.ID = 0;
    return *this;
}

void VAO::LinkAttrib(const GLuint layout, const GLuint numComponents, const GLenum type, const GLsizeiptr stride, const void* offset) {
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
}

void VAO::Bind() const {
    glBindVertexArray(ID);
}

void VAO::Unbind() {
    glBindVertexArray(0);
}

void VAO::Delete() {
    glDeleteVertexArrays(1, &ID);
    ID = 0;
}

VAO::~VAO() {
    Delete();
}
