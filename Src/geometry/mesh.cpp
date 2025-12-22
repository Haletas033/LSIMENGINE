#include "../../include/geometry/mesh.h"

#include "gl/VAO.h"
#include "gl/VAO.h"


// Constructor
Mesh::Mesh(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices) {
    Mesh::vertices = vertices;
    Mesh::indices = indices;

    GenerateTangents();
    setupBuffers();
}

void Mesh::ApplyTransformations() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    modelMatrix = glm::scale(modelMatrix, scale);
}

void Mesh::Draw(Shader& shader, Camera& camera, const glm::mat4 &finalMatrix) {
    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(finalMatrix));
    GLuint normalLoc = glGetUniformLocation(shader.ID, "normalMatrix");
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(finalMatrix)));
    glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    GLuint viewPosLoc = glGetUniformLocation(shader.ID, "viewPos");
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));


    shader.Activate();

    if (useTexture) {
        //Albedo
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);
        //Set the sampler uniform to use texture unit 0
        GLint texLoc = glGetUniformLocation(shader.ID, "albedo");
        glUniform1i(texLoc, 0);

        //Specular
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specMapId);
        //Set the sampler uniform to use texture unit 1
        GLint specMapLoc = glGetUniformLocation(shader.ID, "specular");
        glUniform1i(specMapLoc, 1);

        //Normal
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalMapId);
        //Set the sampler uniform to use texture unit 2
        GLint normalMapLoc = glGetUniformLocation(shader.ID, "normal");
        glUniform1i(normalMapLoc, 2);

        //Emissive
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, emissiveMapId);
        //Set the sampler uniform to use texture unit 3
        GLint emissiveMapLoc = glGetUniformLocation(shader.ID, "emissive");
        glUniform1i(emissiveMapLoc, 3);

        //Emissive Intensity
        glUniform1f(glGetUniformLocation(shader.ID, "emissiveIntensity"), emissiveIntensity);
    }

    vao.Bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::GenerateTangents() {
    constexpr int STRIDE = 8;
    std::vector tangents(vertices.size() / STRIDE, glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i+=3) {
        GLuint i0 = indices[i];
        GLuint i1 = indices[i+1];
        GLuint i2 = indices[i+2];

        auto p0 = glm::vec3(
            vertices[i0 * STRIDE + 0],
            vertices[i0 * STRIDE + 1],
            vertices[i0 * STRIDE + 2]
        );

        auto p1 = glm::vec3(
            vertices[i1 * STRIDE + 0],
            vertices[i1 * STRIDE + 1],
            vertices[i1 * STRIDE + 2]
        );

        auto p2 = glm::vec3(
            vertices[i2 * STRIDE + 0],
            vertices[i2 * STRIDE + 1],
            vertices[i2 * STRIDE + 2]
        );

        auto uv0 = glm::vec2(
            vertices[i0 * STRIDE + 6],
            vertices[i0 * STRIDE + 7]
        );

        auto uv1 = glm::vec2(
            vertices[i1 * STRIDE + 6],
            vertices[i1 * STRIDE + 7]
        );

        auto uv2 = glm::vec2(
            vertices[i2 * STRIDE + 6],
            vertices[i2 * STRIDE + 7]
        );

        const glm::vec3 edge1 = p1 - p0;
        const glm::vec3 edge2 = p2 - p0;
        const glm::vec2 deltaUV1 = uv1 - uv0;
        const glm::vec2 deltaUV2 = uv2 - uv0;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        tangents[i0] += tangent;
        tangents[i1] += tangent;
        tangents[i2] += tangent;
    }

    std::vector<GLfloat> newVertices;
    newVertices.reserve((vertices.size() / STRIDE) * 11);

    for (size_t i = 0; i < vertices.size() / STRIDE; ++i)
    {
        for (int j = 0; j < STRIDE; ++j)
            newVertices.push_back(vertices[i * STRIDE + j]);

        glm::vec3 t = glm::normalize(tangents[i]);
        newVertices.push_back(t.x);
        newVertices.push_back(t.y);
        newVertices.push_back(t.z);
    }

    vertices = std::move(newVertices);
}

void Mesh::setupBuffers() {
    //Generate Vertex Array Object and binds it
    vao.Bind();
    //Generates Vertex Buffer Object and links it to vertices
    VBO vbo(vertices);
    //Generate Element Buffer Object and links it to indices
    EBO ebo(indices);
    //Link VBO attributes such as coordinates and colors to VAO
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 11 * sizeof(float), nullptr);
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 11 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, 11 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    vao.LinkAttrib(vbo, 3, 3, GL_FLOAT, 11 * sizeof(float), reinterpret_cast<void *>(8 * sizeof(float)));
    //Unbind all to prevent accidentally modifying them
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

