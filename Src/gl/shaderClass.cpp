#include "../../include/gl/shaderClass.h"

#include <iostream>

#include "LSIMtypes.h"
#include "utils/logging/log.h"

std::pair<GLuint, std::optional<LSIM::Error>> Shader::CreateShader(const std::optional<std::string> &shaderSource, const int type) {
    if (shaderSource == std::nullopt) return {0, LSIM::Error{LSIM::ErrorCode::SHADER_SOURCE_EMPTY, LSIM::FatalityLevel::FATAL}};
    const GLuint shader = glCreateShader(type);
    const GLchar* src = shaderSource.value().c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar error[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, error);
        engineLogger("stdError", "SHADER COMPILATION FAILED: " + std::string(error));
        glDeleteShader(shader);
        return{0, LSIM::Error{LSIM::ErrorCode::SHADER_COMPILATION_FAILURE, LSIM::FatalityLevel::FATAL}};
    }

    return {shader, std::nullopt};
}

std::pair<Shader, std::optional<LSIM::Error>> Shader::Create(const ShaderFiles& shaders) {
    Shader shader{};

    auto [vsdr, verr] = CreateShader(
        shaders.vertexSource, GL_VERTEX_SHADER);
    if (verr) { return {std::move(shader), verr}; }

    auto [fsdr, ferr] = CreateShader(
        shaders.fragmentSource, GL_FRAGMENT_SHADER);
    if (ferr) {
        SHADER_SAFE_DELETE(vsdr);
        return {std::move(shader), ferr};
    }

    GLuint gsdr = 0;

    if (shaders.geometrySource) {
        auto [geometryShader, gerr] = CreateShader(
            shaders.geometrySource, GL_GEOMETRY_SHADER);

        if (gerr) {
            SHADER_SAFE_DELETE(vsdr);
            SHADER_SAFE_DELETE(fsdr);
            return {std::move(shader), gerr};
        }

        gsdr = geometryShader;
    }

    shader.ID = glCreateProgram();

    SHADER_SAFE_ATTACH(vsdr, shader);
    SHADER_SAFE_ATTACH(fsdr, shader);
    SHADER_SAFE_ATTACH(gsdr, shader);

    glLinkProgram(shader.ID);
    GLint success;
    glGetProgramiv(shader.ID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar error[1024];
        glGetProgramInfoLog(shader.ID, 1024, nullptr, error);
        engineLogger("stdError", "SHADER LINKING FAILED: " + std::string(error));
        SHADER_SAFE_DELETE(vsdr);
        SHADER_SAFE_DELETE(fsdr);
        SHADER_SAFE_DELETE(gsdr);
        shader.Delete();
        return {std::move(shader), LSIM::Error{LSIM::ErrorCode::SHADER_LINK_FAILURE, LSIM::FatalityLevel::FATAL}};
    }

    SHADER_SAFE_DELETE(vsdr);
    SHADER_SAFE_DELETE(fsdr);
    SHADER_SAFE_DELETE(gsdr);
    return {std::move(shader), std::nullopt};
}

Shader::Shader(Shader&& other) noexcept {
    this->ID = other.GetID();
    other.ID = 0;
}

void Shader::Activate() const {
    glUseProgram(ID);
}

GLuint Shader::GetLocation(const std::string &name) const {
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


