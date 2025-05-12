#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>


class VBO {
    public:
        //Reference ID of the VBO
        GLuint ID;
        //Constructor that generates a VBO and links it to vertices
        VBO(std::vector<GLfloat>& vertices);

        //Binds the VBO
        void Bind();
        //Unbinds the VBO
        void Unbind();
        //Deletes the VBO
        void Delete();
};

#endif //VBO_CLASS_H
