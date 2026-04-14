#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>


class VBO {
private:
    GLuint ID = 0;
    void Delete();
public:
    explicit VBO(const std::vector<GLfloat>& vertices);
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;
    VBO(VBO &&other) noexcept;
    VBO& operator=(VBO &&other) noexcept;
    void Bind() const;
    static void Unbind();
    ~VBO();
};

#endif //VBO_CLASS_H
