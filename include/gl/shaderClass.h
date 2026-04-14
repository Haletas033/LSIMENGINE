
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include <optional>

#define SHADER_SAFE_ATTACH(shader) do {if (shader) glAttachShader(ID, shader);} while (0)
#define SHADER_SAFE_DELETE(shader) do {if (shader) glDeleteShader(shader);} while (0)

class Shader {
private:
    GLuint ID = 0;
    void Delete();
    static GLuint CreateShader(const std::optional<std::string> &shaderSource, int type);
public:
    struct ShaderFiles {
        std::optional<std::string> vertexSource;
        std::optional<std::string> fragmentSource;
        std::optional<std::string> geometrySource;
    };

    Shader(const ShaderFiles& shaders);

    void Activate() const;

    ~Shader();
};

#endif //SHADER_CLASS_H
