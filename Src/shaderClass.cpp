#include "../include/shaderClass.h"

//Reads a text file and outputs a string with everything in the file
std::string getFileContent(const char *filename) {
    std::ifstream in(filename, std::ios::binary);

    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    }
    throw(errno);
}

//Constructor that builds the shader program from to different arrays
Shader::Shader(const char *vertexFile, const char *fragmentFile) {

    //Creates strings of the vertex and fragment shaders and stores them
    const std::string vertexCode = getFileContent(vertexFile);
    const std::string fragmentCode = getFileContent(fragmentFile);

    //turns the shader strings into character arrays
    const char* vertexSource= vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    //Creates a vertexShader and gets it's reference
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //Attaches vertexShader source to the Vertex Shader object
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    //Compiles the vertexShader into machine code
    glCompileShader(vertexShader);


    //Creates a fragmentShader and gets it's reference
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //Attaches fragmentShader source to the Fragment shader object
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    //Compiles the fragmentShader into machine code
    glCompileShader(fragmentShader);

    //Creates a shaderProgram object and get its reference
    ID = glCreateProgram();

    //Attaches the vertex and fragment shaders to the shader program
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    //Links all the shaders together into the shader program
    glLinkProgram(ID);

    //Deletes the now useless fragment and vertex shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

//Activates the shader program
void Shader::Activate() {
    glUseProgram(ID);
}

//Deletes the shader program
void Shader::Delete() {
    glDeleteProgram(ID);
}


