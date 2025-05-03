#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO {
    public:
        //Reference ID of the VBO
        GLuint ID;
        //Constructor that generates a VBO and links it to vertices
        VBO(GLfloat* vertices, GLintptr size);

        //Binds the VBO
        void Bind();
        //Unbinds the VBO
        void Unbind();
        //Deletes the VBO
        void Delete();
};

#endif //VBO_CLASS_H
