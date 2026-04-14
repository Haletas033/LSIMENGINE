#include "../../include/gl/VBO.h"

VBO::VBO(const std::vector<GLfloat>& vertices) {
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
}

VBO::VBO(VBO &&other) noexcept {
    ID = other.ID;
    other.ID = 0;
}

VBO &VBO::operator=(VBO &&other) noexcept {
    if (this == &other) return *this;
    Delete();
    ID = other.ID;
    other.ID = 0;
    return *this;
}

void VBO::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
    glDeleteBuffers(1, &ID);
    ID = 0;
}

VBO::~VBO() {
    Delete();
}
