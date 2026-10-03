
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <optional>
#include<glad/glad.h>
#include <glm/glm.hpp>
#include<string>
#include <optional>

#include "LSIMtypes.h"

#define SHADER_SAFE_ATTACH(shader, program) do {if (shader) glAttachShader(program.ID, shader);} while (0)
#define SHADER_SAFE_DELETE(shader) do {if (shader) glDeleteShader(shader);} while (0)

class Shader {
private:
    GLuint ID = 0;
    void Delete();
    [[nodiscard]] static std::pair<GLuint, std::optional<LSIM::Error>> CreateShader(const std::optional<std::string> &shaderSource, int type);
    explicit Shader() = default;

public:
    struct ShaderFiles {
        std::optional<std::string> vertexSource;
        std::optional<std::string> fragmentSource;
        std::optional<std::string> geometrySource;
    };

    [[nodiscard]] static std::pair<Shader, std::optional<LSIM::Error>> Create(const ShaderFiles &shaders);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader &&other) noexcept;

    Shader& operator=(Shader&& other) noexcept {
        this->Delete();
        this->ID = other.GetID();
        other.ID = 0;
        return *this;
    }

    void Activate() const;
    [[nodiscard]] GLuint GetID() const { return ID; }
    [[nodiscard]] GLuint GetLocation(const std::string &name) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetVec4(const std::string &name, int count, const float *value) const;
    void SetVec3(const std::string &name, int count, const float *value) const;
    void SetMat4(const std::string &name, int count, const float *value) const;
    void SetMat3(const std::string &name, int count, const float *value) const;

    ~Shader();
};

#endif //SHADER_CLASS_H
