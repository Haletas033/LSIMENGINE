
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

        //Constructor that builds the shaderProgram from 3 different shaders
        Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile, bool useRawString);

        Shader(const char *vertexFile, const char *fragmentFile, bool useRawString);

        //Activates the shaderProgram
        void Activate();
        //Deletes the shaderProgram
        void Delete();
    private:
        static GLuint CreateShader(const char *shaderSource, int type);
};

#endif //SHADER_CLASS_H
