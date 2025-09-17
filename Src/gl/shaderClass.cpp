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

//Constructor that builds the shader program from to different arrays
Shader::Shader(const char *vertexFile, const char *fragmentFile, const char* geometryFile, bool useRawString) {

    //Creates strings of the vertex and fragment shaders and stores them
    const std::string vertexCode = useRawString ? vertexFile : getFileContent(vertexFile);
    const std::string fragmentCode = useRawString ? fragmentFile : getFileContent(fragmentFile);
    const std::string geometryCode = useRawString ? geometryFile : getFileContent(geometryFile);

    //turns the shader strings into character arrays
    const char* vertexSource= vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();
    const char* geometrySource = geometryCode.c_str();

    //Create a vertexShader and gets it's reference
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //Attach vertexShader source to the Vertex Shader object
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    //Compile the vertexShader into machine code
    glCompileShader(vertexShader);


    //Create a fragmentShader and gets it's reference
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //Attach fragmentShader source to the Fragment shader object
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    //Compile the fragmentShader into machine code
    glCompileShader(fragmentShader);

    //Create a geometryShader and gets it's reference
    const GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    //Attach geometryShader source to the Geometry shader object
    glShaderSource(geometryShader, 1, &geometrySource, nullptr);
    //Compile the geometryShader into machine code
    glCompileShader(geometryShader);

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

//Activates the shader program
void Shader::Activate() {
    glUseProgram(ID);
}

//Deletes the shader program
void Shader::Delete() {
    glDeleteProgram(ID);
}


