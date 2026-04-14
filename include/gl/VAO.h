#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include "VBO.h"

class VAO {
private:
    GLuint ID = 0;
    void Delete();
public:
    VAO();
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;
    VAO(VAO &&other) noexcept;
    VAO& operator=(VAO &&other) noexcept;
    void LinkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, const void *offset);
    void Bind() const;
    static void Unbind();
    ~VAO();
};

#endif //VAO_CLASS_H
