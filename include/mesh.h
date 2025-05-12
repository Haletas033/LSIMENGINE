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



    Mesh(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices);

    void Draw(Shader& shader, Camera& camera);

};
#endif //MESH_CLASS_H
