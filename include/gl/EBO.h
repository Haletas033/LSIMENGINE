#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>
#include<vector>

class EBO {
private:
    GLuint ID = 0;
    void Delete();
public:
    explicit EBO(const std::vector<GLuint>& indices);
    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;
    EBO(EBO &&other) noexcept;
    EBO& operator=(EBO &&other) noexcept;

    void Bind() const;
    static void Unbind();
    ~EBO();
};

#endif //EBO_CLASS_H
