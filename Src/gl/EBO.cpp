#include "../../include/gl/EBO.h"

EBO::EBO(const std::vector<GLuint>& indices) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

EBO::EBO(EBO &&other) noexcept {
    this->ID = other.ID;
    other.ID = 0;
}

EBO &EBO::operator=(EBO &&other) noexcept {
    if (this == &other) return *this;
    Delete();
    ID = other.ID;
    other.ID = 0;
    return *this;
}

void EBO::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() {
    glDeleteBuffers(1, &ID);
    ID = 0;
}

EBO::~EBO() {
    Delete();
}


