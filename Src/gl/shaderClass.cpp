#include "../../include/gl/shaderClass.h"

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

//Create a shader and get its reference
GLuint Shader::CreateShader(const char* shaderSource, const int type) {
    const GLuint shader = glCreateShader(type);
    //Attach vertexShader source to the Vertex Shader object
    glShaderSource(shader, 1, &shaderSource, nullptr);
    //Compile the vertexShader into machine code
    glCompileShader(shader);
    return shader;
}


//Constructor that builds a shader program from 3 shaders
Shader::Shader(const char *vertexFile, const char *fragmentFile, const char* geometryFile, bool useRawString) {

    //Creates strings of the vertex and fragment shaders and stores them
    const std::string vertexCode = useRawString ? vertexFile : getFileContent(vertexFile);
    const std::string fragmentCode = useRawString ? fragmentFile : getFileContent(fragmentFile);
    const std::string geometryCode = useRawString ? geometryFile : getFileContent(geometryFile);

    //turns the shader strings into character arrays
    const char* vertexSource= vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();
    const char* geometrySource = geometryCode.c_str();

    //Create shaders
    const GLuint vertexShader = CreateShader(vertexSource, GL_VERTEX_SHADER);
    const GLuint fragmentShader = CreateShader(fragmentSource, GL_FRAGMENT_SHADER);
    const GLuint geometryShader = CreateShader(geometrySource, GL_GEOMETRY_SHADER);

    //Creates a shaderProgram object and get its reference
    ID = glCreateProgram();

    //Attaches the vertex, fragment and geometry shaders to the shader program
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glAttachShader(ID, geometryShader);
    //Links all the shaders together into the shader program
    glLinkProgram(ID);

    //Deletes the now useless geometry, fragment and vertex shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
}
//Constructor that builds a shader program from 2 shaders
Shader::Shader(const char *vertexFile, const char *fragmentFile, bool useRawString) {

    //Creates strings of the vertex and fragment shaders and stores them
    const std::string vertexCode = useRawString ? vertexFile : getFileContent(vertexFile);
    const std::string fragmentCode = useRawString ? fragmentFile : getFileContent(fragmentFile);

    //turns the shader strings into character arrays
    const char* vertexSource= vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    //Create shaders
    const GLuint vertexShader = CreateShader(vertexSource, GL_VERTEX_SHADER);
    const GLuint fragmentShader = CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

    //Creates a shaderProgram object and get its reference
    ID = glCreateProgram();

    //Attaches the vertex, fragment and geometry shaders to the shader program
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    //Links all the shaders together into the shader program
    glLinkProgram(ID);

    //Deletes the now useless geometry, fragment and vertex shaders
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


