
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<cerrno>

std::string getFileContent(const char* filename);

class Shader {
    public:
        //Reference ID of the shaderProgram
        GLuint ID;
        //Constructor that builds the shaderProgram from 2 different shaders
        Shader(const char* vertexFile, const char* fragmentFile, bool useRawString);

        //Activates the shaderProgram
        void Activate();
        //Deletes the shaderProgram
        void Delete();
};

#endif //SHADER_CLASS_H
