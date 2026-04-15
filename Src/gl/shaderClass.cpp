#include "../../include/gl/shaderClass.h"

#include <iostream>

GLuint Shader::CreateShader(const std::optional<std::string>& shaderSource, const int type) {
    if (shaderSource == std::nullopt) return 0;
    const GLuint shader = glCreateShader(type);
    const GLchar* src = shaderSource.value().c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar error[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, error);
        std::cerr << "SHADER COMPILATION FAILED: " << error << std::endl;
        std::terminate();
    }

    return shader;
}

Shader::Shader(const ShaderFiles& shaders) {
    const GLuint vertexShader = CreateShader(shaders.vertexSource, GL_VERTEX_SHADER);
    const GLuint fragmentShader = CreateShader(shaders.fragmentSource, GL_FRAGMENT_SHADER);
    const GLuint geometryShader = CreateShader(shaders.geometrySource, GL_GEOMETRY_SHADER);

    ID = glCreateProgram();

    SHADER_SAFE_ATTACH(vertexShader);
    SHADER_SAFE_ATTACH(fragmentShader);
    SHADER_SAFE_ATTACH(geometryShader);

    glLinkProgram(ID);
    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar error[1024];
        glGetProgramInfoLog(ID, 1024, nullptr, error);
        std::cerr << "SHADER LINKING FAILED: " << error << std::endl;
        std::terminate();
    }

    SHADER_SAFE_DELETE(vertexShader);
    SHADER_SAFE_DELETE(fragmentShader);
    SHADER_SAFE_DELETE(geometryShader);
}

void Shader::Activate() const {
    glUseProgram(ID);
}

GLint Shader::GetLocation(const std::string &name) const {
    return glGetUniformLocation(ID, name.c_str());
}

void Shader::SetInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetVec4(const std::string &name, const int count, const float* value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), count, value);
}
void Shader::SetVec3(const std::string &name, const int count, const float* value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), count, value);
}
void Shader::SetMat4(const std::string &name, const int count, const float* value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, value);
}
void Shader::SetMat3(const std::string &name, const int count, const float* value) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE, value);
}

void Shader::Delete() {
    glDeleteProgram(ID);
    ID = 0;
}

Shader::~Shader() {
    Delete();
}


